#include <stddef.h>

#include <entry.h>

#include <sys/halt.h>
#include <sys/asm/sti.h>

#include <memory/primary_region.h>
#include <memory/gdt.h>

#include <paging/init.h>
#include <paging/manager.h>

#include <util/heap/heap.h>

#include <interface/interface_map.h>

#include <interrupt/init.h>
#include <interrupt/interrupt_registry.h>

#include <device/device.h>
#include <device/devfs.h>

#include <sysfs/sysfs.h>

#include <timer/timer.h>

#include <event/event.h>

#include <io/arbitrator.h>

#include <filesystem/filesystem.h>
#include <filesystem/ramfs/ramfs.h>

#include <util/string/strlen.h>
#include <util/memory/memcpy.h>
#include <util/math/max.h>

#include <modules/init.h>

#include <process/scheduler.h>

#include <application/application_start_table.h>

#include <pci/pci.h>

#include <entry_error.h>

#include <pkos/defs.h>

#include <debug/vga_print.h>

void user_entry(void);
extern char user_data_start[];

#define VIDEO_MEMORY ((uint8_t *) 0xA0000)

__NORETURN void kernel_main(void) {
    primary_region_init();

    gdt_init();

    paging_init();

    heap_init();

    pman_init();

    if (!paging_init_stage2()) kernel_entry_error(KERNEL_ENTRY_ERROR_PAGING_PHASE_2_ERROR);

    interface_map_init();

    interrupt_init();
    interrupt_registry_init();

    timers_init();

    event_manager_init();

    io_arbitrator_init();

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

    scheduler_sysfs_init();
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

    process_t * init_process = scheduler_queue(
        0,
        MAX(start_table.text_size, 100),
        MAX(start_table.data_size, 100),
        MAX(start_table.rodata_size, 100),
        MAX(start_table.bss_size, 100),
        8192
    );

    vga_print("Load text\n");

    test_file_dirent->superblock->superblock_ops->read(
        test_file_dirent,
        (char *) init_process->text->shared.lender->vaddr,
        start_table.text_size,
        sizeof(application_start_table_t),
        &read_bytes
    );

    // vga_print("RELEASE\n");
    // fs_directory_entry_release(test_file_node->parent_directory_entry);
    // vga_print("DONE\n");

    vga_print("Read text bytes: 0x");
    vga_print_hex(read_bytes);
    vga_print("\n");

    vga_print("Load data\n");

    test_file_dirent->superblock->superblock_ops->read(
        test_file_dirent,
        (char *) init_process->data->shared.lender->vaddr,
        start_table.data_size,
        sizeof(application_start_table_t) + start_table.text_size,
        &read_bytes
    );

    vga_print("Read data bytes: 0x");
    vga_print_hex(read_bytes);
    vga_print("\n");

    vga_print("Load rodata\n");

    test_file_dirent->superblock->superblock_ops->read(
        test_file_dirent,
        (char *) init_process->rodata->shared.lender->vaddr,
        start_table.rodata_size,
        sizeof(application_start_table_t) + start_table.text_size + start_table.data_size,
        &read_bytes
    );

    vga_print("Read rodata bytes: 0x");
    vga_print_hex(read_bytes);
    vga_print("\n");

    fs_directory_entry_t * tty_dirent = fs_open_path(&fs_root, "dev/tty");
    process_file_table_set(&init_process->file_table, stdout, tty_dirent, OPEN_WRITE);

    fs_directory_entry_t * kbd_dirent = fs_open_path(&fs_root, "dev/kbd");
    process_file_table_set(&init_process->file_table, stdin, kbd_dirent, OPEN_READ);

    process_start(init_process);

    vga_print("Starting Init Process\n");
    scheduler_start();
}
