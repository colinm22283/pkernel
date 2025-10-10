#include <stdint.h>

#include <syscall/handlers/exec.h>

#include <filesystem/file.h>

#include <_process/process.h>
#include <_process/scheduler.h>
#include <_process/address_translation.h>

#include <application/application_start_table.h>

#include <util/heap/heap.h>
#include <util/math/max.h>

#include <debug/vga_print.h>

#include <sys/halt.h>

error_number_t syscall_exec(const char * path, const char ** _argv, uint64_t argc) {
    fs_directory_entry_t * test_file_dirent = process_open_path(
        scheduler_current_process(),
        process_user_to_kernel(
            scheduler_current_process(),
            path
        )
    );

    if (test_file_dirent == NULL) {
        return ERROR_FS_NO_ENT;
    }

    if (test_file_dirent->type != FS_REGULAR) {
        fs_directory_entry_release(test_file_dirent);

        return ERROR_NOT_REG;
    }

    uint64_t read_bytes;

    application_start_table_t start_table;
    test_file_dirent->superblock->superblock_ops->read(test_file_dirent, (char *) &start_table, sizeof(application_start_table_t), 0, &read_bytes);

    process_t * current_process = scheduler_current_process();

    current_process->text   = pman_context_resize(current_process->text,   MAX(100, start_table.text_size));
    current_process->data   = pman_context_resize(current_process->data,   MAX(100, start_table.data_size));
    current_process->rodata = pman_context_resize(current_process->rodata, MAX(100, start_table.rodata_size));
    current_process->bss    = pman_context_resize(current_process->bss,    MAX(100, start_table.bss_size));

    pman_context_prepare_write(current_process, current_process->text);

    test_file_dirent->superblock->superblock_ops->read(
        test_file_dirent,
        (char *) get_root_mapping(current_process->text)->vaddr,
        start_table.text_size,
        sizeof(application_start_table_t),
        &read_bytes
    );

    pman_context_prepare_write(current_process, current_process->data);
    test_file_dirent->superblock->superblock_ops->read(
        test_file_dirent,
        (char *) current_process->data->shared.lender->vaddr,
        start_table.data_size,
        sizeof(application_start_table_t) + start_table.text_size,
        &read_bytes
    );

    pman_context_prepare_write(current_process, current_process->rodata);
    test_file_dirent->superblock->superblock_ops->read(
        test_file_dirent,
        (char *) current_process->rodata->shared.lender->vaddr,
        start_table.rodata_size,
        sizeof(application_start_table_t) + start_table.text_size + start_table.data_size,
        &read_bytes
    );

    fs_directory_entry_release(test_file_dirent);

    current_process->thread_table.threads[0]->isr.rip = (uint64_t) current_process->text->vaddr;
    current_process->thread_table.threads[0]->isr.rsp = ((intptr_t) current_process->thread_table.threads[0]->stack->vaddr + current_process->thread_table.threads[0]->stack->size_pages * 0x1000 - 1) & ~0b111;

    if (argc != 0) {
        const char ** argv = process_user_to_kernel(current_process, _argv);

        const char * kern_argv[argc];
        for (uint64_t i = 0; i < argc; i++) {
            kern_argv[i] = process_user_to_kernel(current_process, argv[i]);
        }

        process_push_args(current_process, kern_argv, argc);
    }
    else {
        process_push_args(current_process, NULL, 0);
    }

    return ERROR_OK;
}
