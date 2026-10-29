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

#include <sys/process/push_args.h>

#include <sys/tsr/tsr_load_pc.h>
#include <sys/tsr/tsr_set_stack.h>
#include <sys/paging/page_size.h>

#include <config/exec.h>
#include <config/limits.h>

#ifdef EXEC_DEBUG
#define DEBUG_LOGGER_ENABLED
#endif
#include <debug/debug_logger.h>

DEFINE_KERNEL_PRINTF("exec");

int syscall_exec(const char * _path, const char ** _argv, uint64_t argc) {
    process_t * current_process = scheduler_current_process();

    char path[256];
    long size = process_strncpy_from_user(
        current_process,
        path,
        _path,
        256
    );
    if (size < 0) return size;

    kprintf("exec(%s, ...)", path);

    /* syscall_debug_print("SYSCALL exec("); */
    /* syscall_debug_print(path); */
    /* syscall_debug_print(", "); */
    /* syscall_debug_print_hex((uint64_t) _argv); */
    /* syscall_debug_print(" [ "); */
    /* if (argc != 0) { */
        /* const char ** argv = process_user_to_kernel(scheduler_current_process(), _argv); */

        /* for (uint64_t i = 0; i < argc; i++) { */
            /* if (i != 0) syscall_debug_print(", "); */
            /* syscall_debug_print_hex((uint64_t) argv[i]); */
            /* syscall_debug_print(" ("); */
            /* syscall_debug_print((const char *) process_user_to_kernel(scheduler_current_process(), argv[i])); */
            /* syscall_debug_print(")"); */
        /* } */
    /* } */
    /* syscall_debug_print(" ], "); */
    /* syscall_debug_print_hex(argc); */
    /* syscall_debug_print(")\n"); */

    fs_directory_entry_t * elf_dirent = process_open_path(
        scheduler_current_process(),
        path
    );

    if (elf_dirent == NULL) {
        return -ENOENT;
    }

    if (argc != 0) {
        long result;

        const char * user_argv[argc];
        char ** argv = heap_alloc_debug(argc * sizeof(char *), "exec argv");
        for (size_t i = 0; i < argc; i++) argv[i] = heap_alloc_debug(LIMIT_ARG_LENGTH, "exec argv[]");

        result = process_copy_from_user(current_process, user_argv, _argv, argc * sizeof(const char *));
        if (result < 0) {
            heap_free(argv);
            return -EFAULT;
        }

        for (size_t i = 0; i < argc; i++) {
            result = process_strncpy_from_user(current_process, argv[i], user_argv[i], LIMIT_ARG_LENGTH);
            if (result < 0) {
                heap_free(argv);
                return -EFAULT;
            }
        }

        process_push_args(current_process, (const char **) argv, argc);

        for (size_t i = 0; i < argc; i++) heap_free(argv[i]);
        heap_free(argv);
    }
    else {
        process_push_args(current_process, NULL, 0);
    }

    int load_result = load_program(current_process, elf_dirent);

    if (load_result != 0) return load_result;

    fs_directory_entry_release(elf_dirent);

    tsr_set_stack(
        &current_process->threads[0]->tsr,
        current_process->threads[0]->stack_vaddr,
        current_process->threads[0]->stack_size
    );

    push_main_args(current_process, &current_process->threads[0]->tsr, current_process->argc, current_process->argv);

    return 0;
}
