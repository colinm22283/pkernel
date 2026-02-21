#include <stddef.h>

#include <memory/primary_region.h>

#include <paging/init.h>
#include <paging/manager.h>

#include <util/heap/heap.h>

#include <interrupt/interrupt_registry.h>

#include <process/trampoline.h>
#include <process/process.h>
#include <process/user_vaddrs.h>

#include <scheduler/scheduler.h>

#include <device/device.h>

#include <timer/timer.h>

#include <io/arbitrator.h>

#include <filesystem/filesystem.h>
#include <filesystem/ramfs/ramfs.h>

#include <util/memory/memset.h>

#include <modules/init.h>

#include <module/module.h>

#include <application/application_start_table.h>

#include <prog_loader/prog_loader.h>

#include <tty/tty.h>

#include <elf/elf.h>

#include <debug/printf.h>

#include <entry_error.h>

#include <pkos/defs.h>

#include <sys/setup.h>
#include <sys/interrupt/interrupt_code.h>
#include <sys/process/trampoline.h>

#include <sys/panic.h>

#include <config/init.h>

#ifdef INIT_DEBUG
    #define DEBUG_LOGGER_ENABLED
#endif
#include <debug/debug_logger.h>

DEFINE_KERNEL_PRINTF("init");

#define VIDEO_MEMORY ((uint8_t *) 0xA0000)

void gpf() {
    panic0("GPF ENCOUNTERED");
}

size_t read_dirent(void * cookie, char * buf, size_t size, size_t offset) {
    fs_directory_entry_t * dirent = cookie;

    fs_size_t read_bytes;

    dirent->superblock->superblock_ops->read(dirent, buf, size, offset, &read_bytes);

    return read_bytes;
}

__NORETURN void kernel_main(void) {
    kprintf("Init primary region");
    primary_region_init();

    kprintf("Init paging phase 1");
    paging_init();

    kprintf("Init heap");
    heap_init();

    kprintf("Init interrupt registry");
    interrupt_registry_init();

    interrupt_registry_register((interrupt_code_t) IC_GENERAL_PROTECTION_FAULT, gpf);

    kprintf("Init paging manager");
    pman_init();

    kprintf("Init paging phase 2");
    if (!paging_init_stage2()) kernel_entry_error(KERNEL_ENTRY_ERROR_PAGING_PHASE_2_ERROR);

    kprintf("Init sys phase 2");
    sys_setup_phase2();

    kprintf("Init timers");
    timers_init();

    kprintf("Init io arbitrator");
    io_arbitrator_init();

    kprintf("Init processes");
    processes_init();

    kprintf("Init scheduler");
    scheduler_init();

    kprintf("Init ttys");
    ttys_init();

    kprintf("Init ELF loader");
    elf_init(heap_alloc, heap_free);

    kprintf("Init devices");
    if (!device_init()) kernel_entry_error(KERNEL_ENTRY_ERROR_DEVICE_INIT_ERROR);

    kprintf("Init fs");
    if (!fs_init()) kernel_entry_error(KERNEL_ENTRY_ERROR_FILESYSTEM_INIT_ERROR);

    kprintf("Init ramfs");
    if (!fs_ramfs_init()) kernel_entry_error(KERNEL_ENTRY_ERROR_FILESYSTEM_RAMFS_INIT_ERROR);

    kprintf("Mount ramfs");
    if (fs_mount_root("ramfs", NULL) != ERROR_OK) kernel_entry_error(KERNEL_ENTRY_ERROR_FILESYSTEM_RAMFS_MOUNT_ERROR);

    fs_directory_entry_t * dev_dirent = fs_make(&fs_root, "dev", FS_DIRECTORY);
    fs_directory_entry_t * sys_dirent = fs_make(&fs_root, "sys", FS_DIRECTORY);

    kprintf("Init modules");
    modules_init();

    kprintf("Load static modules");
    if (!static_module_init()) kernel_entry_error(KERNEL_ENTRY_ERROR_MODULE_INIT_ERROR);

    kprintf("Init sysfs");
    scheduler_init_sysfs();
    heap_init_sysfs();

    kprintf("Mount devfs");
    fs_mount("devfs", dev_dirent, NULL);

    kprintf("Mount sysfs");
    fs_mount("sysfs", sys_dirent, NULL);

    fs_directory_entry_release(dev_dirent);
    fs_directory_entry_release(sys_dirent);

    fs_directory_entry_t * disc_dirent = fs_open_path(&fs_root, "dev/disc0");
    device_t * disc_dev = disc_dirent->device;
    fs_directory_entry_release(disc_dirent);

    kprintf("Unmount devfs");
    fs_unmount(dev_dirent);
    fs_directory_entry_release(dev_dirent);

    kprintf("Unmount ramfs");
    fs_unmount(&fs_root);

    kprintf("Mount PKFS");
    fs_mount_root("pkfs", disc_dev);

    kprintf("Remount devfs");
    dev_dirent = fs_open_path(&fs_root, "dev");
    fs_mount("devfs", dev_dirent, NULL);
    fs_directory_entry_release(dev_dirent);

    kprintf("Remount sysfs");
    dev_dirent = fs_open_path(&fs_root, "sys");
    fs_mount("sysfs", dev_dirent, NULL);
    fs_directory_entry_release(dev_dirent);

    kprintf("Load init process");

    process_t * init_process = process_create();

    pman_context_add_shared(init_process->paging_context, PMAN_PROT_EXECUTE | PMAN_PROT_SHARED, kernel_trampoline_mapping, PROCESS_TRAMPOLINE_USER_VADDR);

    process_add_thread(init_process, thread_create_user(init_process->paging_context, init_process));

    fs_directory_entry_t * elf_dirent = fs_open_path(&fs_root, "bin/init");

    load_program(init_process, elf_dirent);

    fs_directory_entry_release(elf_dirent);

    thread_run(init_process->threads[0]);

    kprintf("Start init process");
    scheduler_yield();
}
