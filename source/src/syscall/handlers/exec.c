#include <stdint.h>

#include <syscall/handlers/exec.h>

#include <filesystem/file.h>

#include <process/process.h>
#include <process/user_vaddrs.h>

#include <scheduler/scheduler.h>

#include <application/application_start_table.h>

#include <util/heap/heap.h>
#include <util/math/max.h>

#include <debug/vga_print.h>

#include <sys/halt.h>

#include <sys/tsr/tsr_load_pc.h>
#include <sys/tsr/tsr_set_stack.h>

error_number_t syscall_exec(const char * path, const char ** _argv, uint64_t argc) {
    process_t * current_process = scheduler_current_process();

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

    pman_mapping_t * old_text = pman_context_get_vaddr(current_process->paging_context, PROCESS_TEXT_USER_VADDR);
    pman_mapping_t * old_data = pman_context_get_vaddr(current_process->paging_context, PROCESS_DATA_USER_VADDR);
    pman_mapping_t * old_rodata = pman_context_get_vaddr(current_process->paging_context, PROCESS_RODATA_USER_VADDR);
    pman_mapping_t * old_bss = pman_context_get_vaddr(current_process->paging_context, PROCESS_BSS_USER_VADDR);

    pman_mapping_t * text;
    pman_mapping_t * data;
    pman_mapping_t * rodata;

    if (old_text != NULL) text = pman_context_resize(old_text, MAX(100, start_table.text_size));
    else {
        pman_mapping_t * kernel_mapping = pman_context_add_alloc(pman_kernel_context(), PMAN_PROT_WRITE, NULL, MAX(100, start_table.text_size));
        text = pman_context_add_shared(current_process->paging_context, PMAN_PROT_EXECUTE, kernel_mapping, PROCESS_TEXT_USER_VADDR);
        pman_context_unmap(kernel_mapping);
    }

    if (old_data != NULL) data = pman_context_resize(old_data, MAX(100, start_table.data_size));
    else {
        pman_mapping_t * kernel_mapping = pman_context_add_alloc(pman_kernel_context(), PMAN_PROT_WRITE, NULL, MAX(100, start_table.data_size));
        data = pman_context_add_shared(current_process->paging_context, PMAN_PROT_WRITE, kernel_mapping, PROCESS_DATA_USER_VADDR);
        pman_context_unmap(kernel_mapping);
    }

    if (old_rodata != NULL) rodata = pman_context_resize(old_rodata, MAX(100, start_table.rodata_size));
    else {
        pman_mapping_t * kernel_mapping = pman_context_add_alloc(pman_kernel_context(), PMAN_PROT_WRITE, NULL, MAX(100, start_table.rodata_size));
        rodata = pman_context_add_shared(current_process->paging_context, 0, kernel_mapping, PROCESS_RODATA_USER_VADDR);
        pman_context_unmap(kernel_mapping);
    }

    if (old_bss != NULL) pman_context_resize(old_bss, MAX(100, start_table.bss_size));
    else {
        pman_mapping_t * kernel_mapping = pman_context_add_alloc(pman_kernel_context(), PMAN_PROT_WRITE, NULL, MAX(100, start_table.bss_size));
        pman_context_add_shared(current_process->paging_context, PMAN_PROT_WRITE, kernel_mapping, PROCESS_BSS_USER_VADDR);
        pman_context_unmap(kernel_mapping);
    }

    if (start_table.text_size != 0) {
        pman_context_prepare_write(current_process, text);
        test_file_dirent->superblock->superblock_ops->read(
            test_file_dirent,
            (char *) get_root_mapping(text)->vaddr,
            start_table.text_size,
            sizeof(application_start_table_t),
            &read_bytes
        );
    }

    if (start_table.data_size != 0) {
        pman_context_prepare_write(current_process, data);
        test_file_dirent->superblock->superblock_ops->read(
            test_file_dirent,
            (char *) get_root_mapping(data)->vaddr,
            start_table.data_size,
            sizeof(application_start_table_t) + start_table.text_size,
            &read_bytes
        );
    }

    if (start_table.rodata_size != 0) {
        pman_context_prepare_write(current_process, rodata);
        test_file_dirent->superblock->superblock_ops->read(
            test_file_dirent,
            (char *) get_root_mapping(rodata)->vaddr,
            start_table.rodata_size,
            sizeof(application_start_table_t) + start_table.text_size + start_table.data_size,
            &read_bytes
        );
    }

    fs_directory_entry_release(test_file_dirent);

    tsr_load_pc(&current_process->threads[0]->tsr, PROCESS_TEXT_USER_VADDR);

    tsr_set_stack(
        &current_process->threads[0]->tsr,
        current_process->threads[0]->stack_mapping->vaddr,
        current_process->threads[0]->stack_mapping->size_pages * 0x1000
    );

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
