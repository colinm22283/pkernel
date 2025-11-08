#include <stddef.h>

#include <sys/halt.h>
#include <sys/asm/sti.h>

#include <memory/primary_region.h>

#include <paging/init.h>
#include <paging/manager.h>

#include <util/heap/heap.h>

#include <interface/interface_map.h>

#include <interrupt/init.h>
#include <interrupt/interrupt_registry.h>

#include <process/process.h>
#include <process/user_vaddrs.h>

#include <scheduler/scheduler.h>

#include <device/device.h>
#include <device/devfs.h>

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

#include <application/application_start_table.h>

#include <pci/pci.h>

#include <entry_error.h>

#include <pkos/defs.h>

#include <sys/setup.h>
#include <sys/interrupt/interrupt_code.h>

#include <sys/panic.h>

#include <debug/vga_print.h>

#define VIDEO_MEMORY ((uint8_t *) 0xA0000)

void test() {
    halt();
}

__NORETURN void kernel_main(void) {
    primary_region_init();

    paging_init();

    heap_init();

    interrupt_registry_init();

    interrupt_registry_register(IC_GENERAL_PROTECTION_FAULT, test);

    pman_init();

    if (!paging_init_stage2()) kernel_entry_error(KERNEL_ENTRY_ERROR_PAGING_PHASE_2_ERROR);

    sys_setup_phase2();

    interface_map_init();

    timers_init();

    io_arbitrator_init();

    processes_init();

    scheduler_init();

    pci_init();

    if (!device_init()) kernel_entry_error(KERNEL_ENTRY_ERROR_DEVICE_INIT_ERROR);

    if (!fs_init()) kernel_entry_error(KERNEL_ENTRY_ERROR_FILESYSTEM_INIT_ERROR);
    if (!fs_ramfs_init()) kernel_entry_error(KERNEL_ENTRY_ERROR_FILESYSTEM_RAMFS_INIT_ERROR);

    if (fs_mount_root("ramfs", NULL) != ERROR_OK) kernel_entry_error(KERNEL_ENTRY_ERROR_FILESYSTEM_RAMFS_MOUNT_ERROR);

    fs_directory_entry_t * dev_dirent = fs_make(&fs_root, "dev", FS_DIRECTORY);
    fs_directory_entry_t * sys_dirent = fs_make(&fs_root, "sys", FS_DIRECTORY);

    devfs_init();
    sysfs_init();

    // scheduler_sysfs_init();
    heap_init_sysfs();

    fs_mount("devfs", dev_dirent, NULL);
    fs_mount("sysfs", sys_dirent, NULL);

    fs_directory_entry_release(dev_dirent);
    fs_directory_entry_release(sys_dirent);

    if (!static_module_init()) kernel_entry_error(KERNEL_ENTRY_ERROR_MODULE_INIT_ERROR);

    fs_directory_entry_t * disc_dirent = fs_open_path(&fs_root, "dev/disc0");
    device_t * disc_dev = devfs_open(disc_dirent->node);
    fs_directory_entry_release(disc_dirent);

    fs_unmount(dev_dirent);
    fs_directory_entry_release(dev_dirent);

    fs_unmount(&fs_root);

    fs_mount_root("pkfs", disc_dev);

    vga_print("PKFS Mounted\n");

    dev_dirent = fs_open_path(&fs_root, "dev");
    fs_mount("devfs", dev_dirent, NULL);
    fs_directory_entry_release(dev_dirent);

    vga_print("DevFS Mounted\n");

    dev_dirent = fs_open_path(&fs_root, "sys");
    fs_mount("sysfs", dev_dirent, NULL);
    fs_directory_entry_release(dev_dirent);

    vga_print("SysFS Mounted\n");

    fs_directory_entry_t * test_file_dirent = fs_open_path(&fs_root, "bin/init");

    uint64_t read_bytes;

    vga_print("Load start table\n");

    application_start_table_t start_table;
    test_file_dirent->superblock->superblock_ops->read(test_file_dirent, (char *) &start_table, sizeof(application_start_table_t), 0, &read_bytes);

    vga_print("Load init process\n");

    vga_print("text size:   0x");
    vga_print_hex(start_table.text_size);
    vga_print("\n");

    vga_print("data size:   0x");
    vga_print_hex(start_table.data_size);
    vga_print("\n");

    vga_print("rodata size: 0x");
    vga_print_hex(start_table.rodata_size);
    vga_print("\n");

    vga_print("bss size:    0x");
    vga_print_hex(start_table.bss_size);
    vga_print("\n");

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
        void * process_data = process_create_segment(init_process, PROCESS_DATA_USER_VADDR, start_table.data_size, PMAN_PROT_EXECUTE);

        test_file_dirent->superblock->superblock_ops->read(
            test_file_dirent,
            process_data,
            start_table.data_size,
            sizeof(application_start_table_t) + start_table.text_size,
            &read_bytes
        );
    }

    if (start_table.rodata_size > 0) {
        void * process_rodata = process_create_segment(init_process, PROCESS_RODATA_USER_VADDR, start_table.rodata_size, PMAN_PROT_EXECUTE);

        test_file_dirent->superblock->superblock_ops->read(
            test_file_dirent,
            process_rodata,
            start_table.rodata_size,
            sizeof(application_start_table_t) + start_table.text_size + start_table.data_size,
            &read_bytes
        );
    }

    if (start_table.bss_size > 0) {
        void * process_bss = process_create_segment(init_process, PROCESS_BSS_USER_VADDR, start_table.bss_size, PMAN_PROT_EXECUTE);

        memset(process_bss, 0, start_table.bss_size);
    }

    process_add_thread(init_process, thread_create_user(init_process->paging_context, init_process));

    thread_load_pc(init_process->threads[0], PROCESS_TEXT_USER_VADDR);

    thread_run(init_process->threads[0]);

    vga_print("Starting Init Process\n");
    scheduler_yield();
}
