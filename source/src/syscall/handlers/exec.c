#include <stdint.h>

#include <syscall/syscall_debug.h>

#include <syscall/handlers/exec.h>

#include <filesystem/file.h>

#include <process/process.h>

#include <prog_loader/prog_loader.h>

#include <scheduler/scheduler.h>

#include <elf/elf.h>

#include <debug/printf.h>

#include <util/heap/heap.h>
#include <util/math/max.h>
#include <util/memory/memset.h>

#include <sys/halt.h>

#include <sys/function/push_args.h>

#include <sys/tsr/tsr_load_pc.h>
#include <sys/tsr/tsr_set_stack.h>
#include <sys/paging/page_size.h>

error_number_t syscall_exec(const char * _path, const char ** _argv, uint64_t argc) {
    const char * path = process_user_to_kernel(
        scheduler_current_process(),
        _path
    );

    syscall_debug_print("SYSCALL exec(");
    syscall_debug_print(path);
    syscall_debug_print(", ");
    syscall_debug_print_hex((uint64_t) _argv);
    syscall_debug_print(" [ ");
    if (argc != 0) {
        const char ** argv = process_user_to_kernel(scheduler_current_process(), _argv);

        for (uint64_t i = 0; i < argc; i++) {
            if (i != 0) syscall_debug_print(", ");
            syscall_debug_print_hex((uint64_t) argv[i]);
            syscall_debug_print(" (");
            syscall_debug_print((const char *) process_user_to_kernel(scheduler_current_process(), argv[i]));
            syscall_debug_print(")");
        }
    }
    syscall_debug_print(" ], ");
    syscall_debug_print_hex(argc);
    syscall_debug_print(")\n");

    process_t * current_process = scheduler_current_process();



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

    fs_directory_entry_t * elf_dirent = process_open_path(
        scheduler_current_process(),
        path
    );

    error_number_t load_result = load_program(current_process, elf_dirent);

    if (load_result != ERROR_OK) return load_result;

    fs_directory_entry_release(elf_dirent);

    tsr_set_stack(
        &current_process->threads[0]->tsr,
        current_process->threads[0]->stack_mapping->vaddr,
        current_process->threads[0]->stack_mapping->size_pages * PAGE_SIZE
    );

    push_main_args(current_process, &current_process->threads[0]->tsr, current_process->threads[0]->stack_mapping, current_process->argc, current_process->argv);

    return ERROR_OK;
}
