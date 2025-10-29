KN_OD_64=$(OBJ_DIR)/64/src

export INCLUDE_DIRS+=$(CURDIR)/include
export INCLUDE_DIRS+=$(SOURCE_DIR)/shared/include

KN_OBJS+=$(KN_OD_64)/util/memory/memcpy.o
KN_OBJS+=$(KN_OD_64)/util/memory/memset.o

KN_OBJS+=$(KN_OD_64)/util/string/strcmp.o
KN_OBJS+=$(KN_OD_64)/util/string/strcmpn.o
KN_OBJS+=$(KN_OD_64)/util/string/strlen.o
KN_OBJS+=$(KN_OD_64)/util/string/strcpy.o
KN_OBJS+=$(KN_OD_64)/util/string/writestr.o

KN_OBJS+=$(KN_OD_64)/util/math/pow.o

KN_OBJS+=$(KN_OD_64)/util/heap/init.o
KN_OBJS+=$(KN_OD_64)/util/heap/alloc.o
KN_OBJS+=$(KN_OD_64)/util/heap/free.o
KN_OBJS+=$(KN_OD_64)/util/heap/realloc.o
KN_OBJS+=$(KN_OD_64)/util/heap/usage.o
KN_OBJS+=$(KN_OD_64)/util/heap/internal.o

KN_OBJS+=$(KN_OD_64)/paging/tables.o
KN_OBJS+=$(KN_OD_64)/paging/init.o
KN_OBJS+=$(KN_OD_64)/paging/region.o
KN_OBJS+=$(KN_OD_64)/paging/bitmap.o
KN_OBJS+=$(KN_OD_64)/paging/mapper.o
KN_OBJS+=$(KN_OD_64)/paging/translation_map.o
KN_OBJS+=$(KN_OD_64)/paging/table_allocator.o
KN_OBJS+=$(KN_OD_64)/paging/virtual_allocator.o
KN_OBJS+=$(KN_OD_64)/paging/physical_allocator.o
KN_OBJS+=$(KN_OD_64)/paging/manager.o

KN_OBJS+=$(KN_OD_64)/memory/memory_map.o
KN_OBJS+=$(KN_OD_64)/memory/primary_region.o

KN_OBJS+=$(KN_OD_64)/task/enter_user_mode.o

KN_OBJS+=$(KN_OD_64)/syscall/syscall_handler.o
KN_OBJS+=$(KN_OD_64)/syscall/handlers/open.o
#KN_OBJS+=$(KN_OD_64)/syscall/handlers/close.o
KN_OBJS+=$(KN_OD_64)/syscall/handlers/write.o
KN_OBJS+=$(KN_OD_64)/syscall/handlers/read.o
#KN_OBJS+=$(KN_OD_64)/syscall/handlers/seek.o
KN_OBJS+=$(KN_OD_64)/syscall/handlers/exit.o
KN_OBJS+=$(KN_OD_64)/syscall/handlers/readdir.o
KN_OBJS+=$(KN_OD_64)/syscall/handlers/chdir.o
KN_OBJS+=$(KN_OD_64)/syscall/handlers/fork.o
KN_OBJS+=$(KN_OD_64)/syscall/handlers/exec.o
KN_OBJS+=$(KN_OD_64)/syscall/handlers/map.o
#KN_OBJS+=$(KN_OD_64)/syscall/handlers/pipe.o
KN_OBJS+=$(KN_OD_64)/syscall/handlers/dup.o
#KN_OBJS+=$(KN_OD_64)/syscall/handlers/mount.o
#KN_OBJS+=$(KN_OD_64)/syscall/handlers/unmount.o
#KN_OBJS+=$(KN_OD_64)/syscall/handlers/mkdir.o
#KN_OBJS+=$(KN_OD_64)/syscall/handlers/remove.o

KN_OBJS+=$(KN_OD_64)/interface/interface.o
KN_OBJS+=$(KN_OD_64)/interface/interface_map.o
KN_OBJS+=$(KN_OD_64)/interface/fifo.o

#KN_OBJS+=$(KN_OD_64)/filesystem/filesystem.o
#KN_OBJS+=$(KN_OD_64)/filesystem/filesystem_table.o
#KN_OBJS+=$(KN_OD_64)/filesystem/directory_entries.o
#KN_OBJS+=$(KN_OD_64)/filesystem/file.o
#KN_OBJS+=$(KN_OD_64)/filesystem/mapping_registry.o
#KN_OBJS+=$(KN_OD_64)/filesystem/node_cache.o
#KN_OBJS+=$(KN_OD_64)/filesystem/node_number_allocator.o
#KN_OBJS+=$(KN_OD_64)/filesystem/ramfs/ramfs.o
#KN_OBJS+=$(KN_OD_64)/filesystem/ramfs/superblock_ops.o
#KN_OBJS+=$(KN_OD_64)/filesystem/ramfs/node_ops.o

KN_OBJS+=$(KN_OD_64)/filesystem/filesystem.o
KN_OBJS+=$(KN_OD_64)/filesystem/directory_entry.o
KN_OBJS+=$(KN_OD_64)/filesystem/superblock.o
KN_OBJS+=$(KN_OD_64)/filesystem/node.o
KN_OBJS+=$(KN_OD_64)/filesystem/pipe.o
KN_OBJS+=$(KN_OD_64)/filesystem/file.o
KN_OBJS+=$(KN_OD_64)/filesystem/ramfs/ramfs.o

KN_OBJS+=$(KN_OD_64)/io/arbitrator.o

KN_OBJS+=$(KN_OD_64)/timer/timer.o

KN_OBJS+=$(KN_OD_64)/pci/pci.o

#KN_OBJS+=$(KN_OD_64)/process/process.o
#KN_OBJS+=$(KN_OD_64)/process/scheduler.o
#KN_OBJS+=$(KN_OD_64)/process/address_translation.o
#KN_OBJS+=$(KN_OD_64)/process/file_table.o
#KN_OBJS+=$(KN_OD_64)/process/thread_table.o
#KN_OBJS+=$(KN_OD_64)/process/signal.o
#KN_OBJS+=$(KN_OD_64)/process/signal_trampoline.o

KN_OBJS+=$(KN_OD_64)/process/process.o
KN_OBJS+=$(KN_OD_64)/process/thread.o
KN_OBJS+=$(KN_OD_64)/process/file_table.o

KN_OBJS+=$(KN_OD_64)/scheduler/scheduler.o
KN_OBJS+=$(KN_OD_64)/scheduler/core.o
KN_OBJS+=$(KN_OD_64)/scheduler/event.o

KN_OBJS+=$(KN_OD_64)/device/device.o
KN_OBJS+=$(KN_OD_64)/device/devfs.o

KN_OBJS+=$(KN_OD_64)/sysfs/sysfs.o
KN_OBJS+=$(KN_OD_64)/sysfs/ops.o
KN_OBJS+=$(KN_OD_64)/sysfs/mount.o

#KN_OBJS+=$(KN_OD_64)/interrupt/init.o
#KN_OBJS+=$(KN_OD_64)/interrupt/idt.o
#KN_OBJS+=$(KN_OD_64)/interrupt/handler_entries.o
KN_OBJS+=$(KN_OD_64)/interrupt/interrupt_registry.o
#KN_OBJS+=$(KN_OD_64)/interrupt/handlers/div0.o
#KN_OBJS+=$(KN_OD_64)/interrupt/handlers/nmi.o
#KN_OBJS+=$(KN_OD_64)/interrupt/handlers/bp_int3.o
#KN_OBJS+=$(KN_OD_64)/interrupt/handlers/ovf.o
#KN_OBJS+=$(KN_OD_64)/interrupt/handlers/bound_range.o
#KN_OBJS+=$(KN_OD_64)/interrupt/handlers/invalid_opcode.o
#KN_OBJS+=$(KN_OD_64)/interrupt/handlers/device_not_avail.o
#KN_OBJS+=$(KN_OD_64)/interrupt/handlers/double_fault.o
#KN_OBJS+=$(KN_OD_64)/interrupt/handlers/coproc_segment_overrun.o
#KN_OBJS+=$(KN_OD_64)/interrupt/handlers/invalid_tss.o
#KN_OBJS+=$(KN_OD_64)/interrupt/handlers/segment_not_present.o
#KN_OBJS+=$(KN_OD_64)/interrupt/handlers/stack_segment_fault.o
#KN_OBJS+=$(KN_OD_64)/interrupt/handlers/general_protection_fault.o
#KN_OBJS+=$(KN_OD_64)/interrupt/handlers/page_fault.o
#KN_OBJS+=$(KN_OD_64)/interrupt/handlers/x87_fpu.o
#KN_OBJS+=$(KN_OD_64)/interrupt/handlers/alignment_check.o
#KN_OBJS+=$(KN_OD_64)/interrupt/handlers/machine_check.o
#KN_OBJS+=$(KN_OD_64)/interrupt/handlers/simd_fpu_error.o
#KN_OBJS+=$(KN_OD_64)/interrupt/handlers/timer_handler.o
#KN_OBJS+=$(KN_OD_64)/interrupt/handlers/keyboard_handler.o

KN_OBJS+=$(KN_OD_64)/main.o
KN_OBJS+=$(KN_OD_64)/entry_error.o

OPTIMIZATION=3

.DEFAULT: all
.PHONY: all
all: kernel modules

.PHONY: kernel
kernel: $(BIN_DIR)/kernel.bin

include modules/module.mk

include $(MAKE_SCRIPTS)

$(BIN_DIR)/kernel.bin: $(STATIC_MODULE_OBJS) $(STATIC_MODULE_HDRS) $(KN_OBJS) $(SOURCE_DIR)/kernel.ld
	mkdir -p $(@D)
	$(LD64) $(LDFLAGS) -T$(SOURCE_DIR)/kernel.ld $(KN_OBJS) $(STATIC_MODULE_OBJS) -o $@ -Map=$@.map

.PHONY: modules
modules: $(MODULE_TARGETS)
