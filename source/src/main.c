#include <stddef.h>

#include <sys/halt.h>
#include <sys/asm/sti.h>

#include <memory/primary_region.h>

#include <paging/init.h>
#include <paging/manager.h>

#include <util/heap/heap.h>

#include <interrupt/init.h>
#include <interrupt/interrupt_registry.h>

#include <process/process.h>
#include <process/user_vaddrs.h>

#include <scheduler/scheduler.h>

#include <device/device.h>

#include <devfs/devfs.h>

#include <sysfs/sysfs.h>

#include <timer/timer.h>

#include <io/arbitrator.h>

#include <filesystem/filesystem.h>
#include <filesystem/ramfs/ramfs.h>

#include <util/string/strlen.h>
#include <util/memory/memcpy.h>
#include <util/memory/memset.h>
#include <util/math/max.h>

#include <modules/init.h>

#include <scheduler/scheduler.h>

#include <module/module.h>

#include <application/application_start_table.h>

#include <pci/pci.h>

#include <entry_error.h>

#include <pkos/defs.h>

#include <sys/setup.h>
#include <sys/interrupt/interrupt_code.h>

#include <sys/panic.h>

#include <config/init.h>

#ifdef INIT_DEBUG
    #define DEBUG_LOGGER_ENABLED
#endif
#include <debug/debug_logger.h>

DEFINE_DEBUG_LOGGER("init");

#define VIDEO_MEMORY ((uint8_t *) 0xA0000)

void gpf() {
    panic0("GPF ENCOUNTERED");

    halt();
}

__NORETURN void kernel_main(void) {
    DEBUG_LOG(DEBUG_PRINT("Init primary region"));
    primary_region_init();

    DEBUG_LOG(DEBUG_PRINT("Init paging phase 1"));
    paging_init();

    DEBUG_LOG(DEBUG_PRINT("Init heap"));
    heap_init();

    DEBUG_LOG(DEBUG_PRINT("Init interrupt registry"));
    interrupt_registry_init();

    interrupt_registry_register((interrupt_code_t) IC_GENERAL_PROTECTION_FAULT, gpf);

    DEBUG_LOG(DEBUG_PRINT("Init paging manager"));
    pman_init();

    DEBUG_LOG(DEBUG_PRINT("Init paging phase 2"));
    if (!paging_init_stage2()) kernel_entry_error(KERNEL_ENTRY_ERROR_PAGING_PHASE_2_ERROR);

    DEBUG_LOG(DEBUG_PRINT("Init sys phase 2"));
    sys_setup_phase2();

    DEBUG_LOG(DEBUG_PRINT("Init timers"));
    timers_init();

    DEBUG_LOG(DEBUG_PRINT("Init io arbitrator"));
    io_arbitrator_init();

    DEBUG_LOG(DEBUG_PRINT("Init processes"));
    processes_init();

    DEBUG_LOG(DEBUG_PRINT("Init scheduler"));
    scheduler_init();

    DEBUG_LOG(DEBUG_PRINT("Init devices"));
    if (!device_init()) kernel_entry_error(KERNEL_ENTRY_ERROR_DEVICE_INIT_ERROR);

    DEBUG_LOG(DEBUG_PRINT("Init fs"));
    if (!fs_init()) kernel_entry_error(KERNEL_ENTRY_ERROR_FILESYSTEM_INIT_ERROR);

    DEBUG_LOG(DEBUG_PRINT("Init ramfs"));
    if (!fs_ramfs_init()) kernel_entry_error(KERNEL_ENTRY_ERROR_FILESYSTEM_RAMFS_INIT_ERROR);

    DEBUG_LOG(DEBUG_PRINT("Mount ramfs"));
    if (fs_mount_root("ramfs", NULL) != ERROR_OK) kernel_entry_error(KERNEL_ENTRY_ERROR_FILESYSTEM_RAMFS_MOUNT_ERROR);

    fs_directory_entry_t * dev_dirent = fs_make(&fs_root, "dev", FS_DIRECTORY);
    fs_directory_entry_t * sys_dirent = fs_make(&fs_root, "sys", FS_DIRECTORY);

    DEBUG_LOG(DEBUG_PRINT("Init modules"));
    modules_init();

    DEBUG_LOG(DEBUG_PRINT("Load static modules"));
    if (!static_module_init()) kernel_entry_error(KERNEL_ENTRY_ERROR_MODULE_INIT_ERROR);

    DEBUG_LOG(DEBUG_PRINT("Init sysfs"));
    scheduler_init_sysfs();
    heap_init_sysfs();

    DEBUG_LOG(DEBUG_PRINT("Mount devfs"));
    fs_mount("devfs", dev_dirent, NULL);

    DEBUG_LOG(DEBUG_PRINT("Mount sysfs"));
    fs_mount("sysfs", sys_dirent, NULL);

    fs_directory_entry_release(dev_dirent);
    fs_directory_entry_release(sys_dirent);

    fs_directory_entry_t * disc_dirent = fs_open_path(&fs_root, "dev/disc0");
    device_t * disc_dev = disc_dirent->device;
    fs_directory_entry_release(disc_dirent);

    DEBUG_LOG(DEBUG_PRINT("Unmount devfs"));
    fs_unmount(dev_dirent);
    fs_directory_entry_release(dev_dirent);

    DEBUG_LOG(DEBUG_PRINT("Unmount ramfs"));
    fs_unmount(&fs_root);

    DEBUG_LOG(DEBUG_PRINT("Mount PKFS"));
    fs_mount_root("pkfs", disc_dev);

    DEBUG_LOG(DEBUG_PRINT("Remount devfs"));
    dev_dirent = fs_open_path(&fs_root, "dev");
    fs_mount("devfs", dev_dirent, NULL);
    fs_directory_entry_release(dev_dirent);

    DEBUG_LOG(DEBUG_PRINT("Remount sysfs"));
    dev_dirent = fs_open_path(&fs_root, "sys");
    fs_mount("sysfs", dev_dirent, NULL);
    fs_directory_entry_release(dev_dirent);

    DEBUG_LOG(DEBUG_PRINT("Load init process"));
    fs_directory_entry_t * test_file_dirent = fs_open_path(&fs_root, "bin/init");

    uint64_t read_bytes;

    application_start_table_t start_table;
    test_file_dirent->superblock->superblock_ops->read(test_file_dirent, (char *) &start_table, sizeof(application_start_table_t), 0, &read_bytes);

    process_t * init_process = process_create();

    if (start_table.text_size > 0) {
        void * process_text = process_create_segment(init_process, PROCESS_TEXT_USER_VADDR, start_table.text_size, PMAN_PROT_EXECUTE);
        test_file_dirent->superblock->superblock_ops->read(
            test_file_dirent,
            process_text,
            start_table.text_size,
            sizeof(application_start_table_t),
            &read_bytes
        );
    }

    if (start_table.data_size > 0) {
        void * process_data = process_create_segment(init_process, PROCESS_DATA_USER_VADDR, start_table.data_size, PMAN_PROT_WRITE);

        test_file_dirent->superblock->superblock_ops->read(
            test_file_dirent,
            process_data,
            start_table.data_size,
            sizeof(application_start_table_t) + start_table.text_size,
            &read_bytes
        );
    }

    if (start_table.rodata_size > 0) {
        void * process_rodata = process_create_segment(init_process, PROCESS_RODATA_USER_VADDR, start_table.rodata_size, 0);

        test_file_dirent->superblock->superblock_ops->read(
            test_file_dirent,
            process_rodata,
            start_table.rodata_size,
            sizeof(application_start_table_t) + start_table.text_size + start_table.data_size,
            &read_bytes
        );
    }

    if (start_table.bss_size > 0) {
        void * process_bss = process_create_segment(init_process, PROCESS_BSS_USER_VADDR, start_table.bss_size, PMAN_PROT_WRITE);

        memset(process_bss, 0, start_table.bss_size);
    }

    process_add_thread(init_process, thread_create_user(init_process->paging_context, init_process));

    thread_load_pc(init_process->threads[0], PROCESS_TEXT_USER_VADDR);

    thread_run(init_process->threads[0]);

    DEBUG_LOG(DEBUG_PRINT("Start init process"));
    scheduler_yield();
}
