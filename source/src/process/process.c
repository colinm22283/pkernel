#include <stddef.h>

#include <process/process.h>
#include <process/scheduler.h>
#include <process/user_vaddrs.h>
#include <process/address_translation.h>

#include <sysfs/sysfs.h>

#include <util/heap/heap.h>

#include <util/memory/memcpy.h>

#include <util/string/writestr.h>

#include <defs.h>

#include <debug/vga_print.h>

#include <sys/asm/sti.h>
#include <sys/asm/cli.h>

#include <sys/wait_for_interrupt.h>

enum {
    SYSFS_PPID = 0b0000,
};

#define SYSFS_PROC_ID(pid, key) (((pid) << 4) | (key))
#define SYSFS_PROC_PID(id) ((id) >> 4)
#define SYSFS_PROC_KEY(id) ((id) & 0b1111)

int64_t proc_sysfs_read(uint64_t id, char * data, uint64_t size, uint64_t offset) {
    if (offset != 0) return 0;

    pid_t pid = SYSFS_PROC_PID(id);

    switch (SYSFS_PROC_KEY(id)) {
        case SYSFS_PPID: {
            process_t * process = scheduler_get_id(pid);

            if (process == NULL) return 0;

            return (int64_t) writestr(data, size, offset, process->parent_id);
        }

        default: break;
    }
    return 0;
}

int64_t proc_sysfs_write(uint64_t id, const char * data, uint64_t size, uint64_t offset) {
    switch (id) {
        default: break;
    }

    return 0;
}

void process_init(
    process_t * process,
    process_id_t parent_id,
    process_id_t id,
    uint64_t text_size,
    uint64_t data_size,
    uint64_t rodata_size,
    uint64_t bss_size,
    uint64_t stack_size
) {
    process->state = PS_STOPPED;
    process->id = id;
    process->parent_id = parent_id;

    process->text_size = text_size;
    process->data_size = data_size;
    process->rodata_size = rodata_size;
    process->bss_size = bss_size;
    process->stack_size = stack_size;

    process->paging_context = pman_new_context();

    pman_mapping_t * temp_mapping;

    temp_mapping = pman_context_add_alloc(pman_kernel_context(), PMAN_PROT_WRITE | PMAN_PROT_EXECUTE, NULL, text_size);
    process->text = pman_context_add_shared(process->paging_context, PMAN_PROT_EXECUTE, temp_mapping, PROCESS_TEXT_USER_VADDR);
    pman_context_unmap(temp_mapping);

    temp_mapping = pman_context_add_alloc(pman_kernel_context(), PMAN_PROT_WRITE, NULL, data_size);
    process->data = pman_context_add_shared(process->paging_context, PMAN_PROT_WRITE, temp_mapping, PROCESS_DATA_USER_VADDR);
    pman_context_unmap(temp_mapping);

    temp_mapping = pman_context_add_alloc(pman_kernel_context(), PMAN_PROT_WRITE, NULL, rodata_size);
    process->rodata = pman_context_add_shared(process->paging_context, 0, temp_mapping, PROCESS_RODATA_USER_VADDR);
    pman_context_unmap(temp_mapping);

    temp_mapping = pman_context_add_alloc(pman_kernel_context(), PMAN_PROT_WRITE, NULL, bss_size);
    process->bss = pman_context_add_shared(process->paging_context, PMAN_PROT_WRITE, temp_mapping, PROCESS_BSS_USER_VADDR);
    pman_context_unmap(temp_mapping);

    process_file_table_init(&process->file_table);

    process_thread_table_init(&process->thread_table, process);

    signal_table_init(&process->signal_table);

    process->working_dir = &fs_root;
    fs_directory_entry_add_reference(process->working_dir);

    process_thread_table_create_thread(&process->thread_table, stack_size);

    process->argc = 0;
    process->argv = NULL;

    char path[40] = "proc/";
    uint64_t size = writestr(path + 5, 20, 0, process->id);
    strcpy(&path[5 + size], "/ppid");
    sysfs_add_entry(path, SYSFS_PROC_ID(process->id, SYSFS_PPID), proc_sysfs_read, proc_sysfs_write);
}

void process_init_fork(
    process_t * process,
    process_t * parent,
    process_id_t id
) {
    process->state = PS_STOPPED;
    process->id = id;
    process->parent_id = parent->id;

    process->text_size = parent->text_size;
    process->data_size = parent->data_size;
    process->rodata_size = parent->rodata_size;
    process->bss_size = parent->bss_size;
    process->stack_size = parent->stack_size;

    process->paging_context = pman_new_context();

    process->text = pman_context_add_borrowed(process->paging_context, PMAN_PROT_EXECUTE, parent->text, PROCESS_TEXT_USER_VADDR);

    process->data = pman_context_add_borrowed(process->paging_context, PMAN_PROT_WRITE, parent->data, PROCESS_DATA_USER_VADDR);

    process->rodata = pman_context_add_borrowed(process->paging_context, 0, parent->rodata, PROCESS_RODATA_USER_VADDR);

    process->bss = pman_context_add_borrowed(process->paging_context, PMAN_PROT_WRITE, parent->bss, PROCESS_BSS_USER_VADDR);

    process_file_table_clone(&process->file_table, &parent->file_table);

    process_thread_table_init(&process->thread_table, process);

    signal_table_init(&process->signal_table);

    fs_directory_entry_add_reference(parent->working_dir);
    process->working_dir = parent->working_dir;

    process_thread_table_create_thread_fork(&process->thread_table, parent, parent->thread_table.threads[0]);

    if (parent->argc != 0) {
        process->argc = parent->argc;
        process->argv = heap_alloc(process->argc * sizeof(const char *));
        memcpy(process->argv, parent->argv, process->argc * sizeof(const char *));
    }
    else {
        process->argc = 0;
        process->argv = NULL;
    }

    char path[40] = "proc/";
    uint64_t size = writestr(path + 5, 20, 0, process->id);
    strcpy(&path[5 + size], "/ppid");
    sysfs_add_entry(path, SYSFS_PROC_ID(process->id, SYSFS_PPID), proc_sysfs_read, proc_sysfs_write);
}

void process_free(process_t * process) {
    char path[40] = "proc/";
    uint64_t size = writestr(path + 5, 20, 0, process->id);
    strcpy(&path[5 + size], "/ppid");
    sysfs_remove_entry(path);

    // TODO: finish
    if (process->argc != 0) heap_free(process->argv);

    fs_directory_entry_release(process->working_dir);

    signal_table_free(&process->signal_table);

    process_thread_table_free(&process->thread_table);

    process_file_table_free(&process->file_table);

    pman_free_context(process->paging_context);
}

void process_push_args(process_t * process, const char ** argv, uint64_t argc) {
    if (process->argc != 0) {
        heap_free(process->argv);
    }

    if (argc == 0) {
        process->argc = 0;
        process->argv = NULL;
    }
    else {
        process->argc = argc;
        process->argv = heap_alloc(argc * sizeof(const char *));
        
        uint64_t required_size = 0;
        for (uint64_t i = 0; i < argc; i++) {
            required_size += strlen(argv[i]) + 1;
        }

        pman_mapping_t * kern_mapping = pman_context_add_alloc(
            pman_kernel_context(),
            0,
            NULL,
            required_size
        );

        pman_mapping_t * user_mapping = pman_context_add_shared(
            process->paging_context,
            0,
            kern_mapping,
            NULL
        );

        char * kern_buf = kern_mapping->vaddr;
        char * user_buf = user_mapping->vaddr;
        
        uint64_t pos = 0;
        for (uint64_t i = 0; i < argc; i++) {
            uint64_t len = strlen(argv[i]);

            process->argv[i] = &user_buf[pos];

            memcpy(&kern_buf[pos], argv[i], len + 1);
            pos += len + 1;
        }

        pman_context_unmap(kern_mapping);
    }

    uint64_t * kern_rsp = process_user_to_kernel(
        process,
        (void *) process->thread_table.threads[0]->isr.rsp
    );

    kern_rsp -= argc;

    for (uint64_t i = 0; i < process->argc; i++) {
        kern_rsp[i] = (uint64_t) process->argv[i];
    }
    kern_rsp--;
    *kern_rsp = process->argc;

    process->thread_table.threads[0]->isr.rsp -= sizeof(uint64_t) * (1 + argc);
}

void process_load_isr(process_t * process, interrupt_state_record_t * isr) {
    memcpy(&process->thread_table.threads[process->thread_table.current_index]->isr, isr, sizeof(interrupt_state_record_t));
}

void process_set_working_dir(process_t * process, fs_directory_entry_t * directory_entry) {
    fs_directory_entry_release(process->working_dir);

    process->working_dir = directory_entry;
}

void process_start(process_t * process) {
    process->state = PS_RUNNING;
}

void process_kill(process_t * process) {
    process->state = PS_DEAD;
}

fs_directory_entry_t * process_open_path(process_t * process, const char * path) {
    if (path[0] == '/') {
        if (path[1] == '\0') return &fs_root;
        else return fs_open_path(&fs_root, path + 1);
    }
    else {
        return fs_open_path(process->working_dir, path);
    }
}

fs_directory_entry_t * process_make_path(process_t * process, const char * path, fs_file_type_t type) {
    if (path[0] == '/') {
        if (path[1] == '\0') return &fs_root;
        else return fs_make_path(&fs_root, path + 1, type);
    }
    else return fs_make_path(process->working_dir, path, type);
}

