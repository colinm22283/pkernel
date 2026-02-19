KN_OD_64=$(OBJ_DIR)/64/src
KN_CD=$(CONFIG_DIR)/kernel/config

export INCLUDE_DIRS+=$(CURDIR)/include
export INCLUDE_DIRS+=$(SOURCE_DIR)/shared/include
export INCLUDE_DIRS+=$(CONFIG_DIR)/kernel

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

KN_OBJS+=$(KN_OD_64)/socket/unix.o

KN_OBJS+=$(KN_OD_64)/syscall/syscall_handler.o
KN_OBJS+=$(KN_OD_64)/syscall/syscall_names.o

KN_OBJS+=$(KN_OD_64)/syscall/handlers/open.o
KN_OBJS+=$(KN_OD_64)/syscall/handlers/close.o
KN_OBJS+=$(KN_OD_64)/syscall/handlers/write.o
KN_OBJS+=$(KN_OD_64)/syscall/handlers/read.o
#KN_OBJS+=$(KN_OD_64)/syscall/handlers/seek.o
KN_OBJS+=$(KN_OD_64)/syscall/handlers/exit.o
KN_OBJS+=$(KN_OD_64)/syscall/handlers/readdir.o
KN_OBJS+=$(KN_OD_64)/syscall/handlers/chdir.o
KN_OBJS+=$(KN_OD_64)/syscall/handlers/fork.o
KN_OBJS+=$(KN_OD_64)/syscall/handlers/exec.o
KN_OBJS+=$(KN_OD_64)/syscall/handlers/map.o
KN_OBJS+=$(KN_OD_64)/syscall/handlers/pipe.o
KN_OBJS+=$(KN_OD_64)/syscall/handlers/dup.o
KN_OBJS+=$(KN_OD_64)/syscall/handlers/mount.o
KN_OBJS+=$(KN_OD_64)/syscall/handlers/unmount.o
KN_OBJS+=$(KN_OD_64)/syscall/handlers/mkdir.o
KN_OBJS+=$(KN_OD_64)/syscall/handlers/remove.o
KN_OBJS+=$(KN_OD_64)/syscall/handlers/socket.o
KN_OBJS+=$(KN_OD_64)/syscall/handlers/connect.o
KN_OBJS+=$(KN_OD_64)/syscall/handlers/bind.o
KN_OBJS+=$(KN_OD_64)/syscall/handlers/accept.o
KN_OBJS+=$(KN_OD_64)/syscall/handlers/listen.o
KN_OBJS+=$(KN_OD_64)/syscall/handlers/signal.o
KN_OBJS+=$(KN_OD_64)/syscall/handlers/signalret.o
KN_OBJS+=$(KN_OD_64)/syscall/handlers/alarm.o
KN_OBJS+=$(KN_OD_64)/syscall/handlers/kill.o

KN_OBJS+=$(KN_OD_64)/filesystem/filesystem.o
KN_OBJS+=$(KN_OD_64)/filesystem/directory_entry.o
KN_OBJS+=$(KN_OD_64)/filesystem/superblock.o
KN_OBJS+=$(KN_OD_64)/filesystem/node.o
KN_OBJS+=$(KN_OD_64)/filesystem/pipe.o
KN_OBJS+=$(KN_OD_64)/filesystem/socket.o
KN_OBJS+=$(KN_OD_64)/filesystem/file.o
KN_OBJS+=$(KN_OD_64)/filesystem/ramfs/ramfs.o

KN_OBJS+=$(KN_OD_64)/io/arbitrator.o

KN_OBJS+=$(KN_OD_64)/timer/timer.o

KN_OBJS+=$(KN_OD_64)/process/process.o
KN_OBJS+=$(KN_OD_64)/process/thread.o
KN_OBJS+=$(KN_OD_64)/process/file_table.o

KN_OBJS+=$(KN_OD_64)/scheduler/scheduler.o
KN_OBJS+=$(KN_OD_64)/scheduler/core.o
KN_OBJS+=$(KN_OD_64)/scheduler/event.o

KN_OBJS+=$(KN_OD_64)/device/device.o

KN_OBJS+=$(KN_OD_64)/signal/signal.o
KN_OBJS+=$(KN_OD_64)/signal/signal_defaults.o

KN_OBJS+=$(KN_OD_64)/module/module.o

KN_OBJS+=$(KN_OD_64)/interrupt/interrupt_registry.o

KN_OBJS+=$(KN_OD_64)/tty/tty.o

KN_OBJS+=$(KN_OD_64)/main.o
KN_OBJS+=$(KN_OD_64)/entry_error.o

CONFIG_TARGETS+=$(KN_CD)/heap.h
CONFIG_TARGETS+=$(KN_CD)/syscall.h
CONFIG_TARGETS+=$(KN_CD)/module.h
CONFIG_TARGETS+=$(KN_CD)/init.h
CONFIG_TARGETS+=$(KN_CD)/pman.h
CONFIG_TARGETS+=$(KN_CD)/tty.h

OPTIMIZATION=3

export AUTOGEN_HEADERS+=$(CONFIG_TARGETS)

.DEFAULT: all
.PHONY: all
all: kernel modules

.PHONY: kernel
kernel: $(BIN_DIR)/kernel.bin $(BIN_DIR)/kernel.sym

include $(MOD_SOURCE_DIR)/module.mk

include $(MAKE_SCRIPTS)

$(BIN_DIR)/kernel.bin: $(AUTOGEN_HEADERS) $(STATIC_MODULE_OBJS) $(STATIC_MODULE_HDRS) $(KN_OBJS) $(SOURCE_DIR)/kernel.ld
	mkdir -p $(@D)
	$(LD64) --oformat binary $(LDFLAGS) -T$(SOURCE_DIR)/kernel.ld $(KN_OBJS) $(STATIC_MODULE_OBJS) -o $@ -Map=$@.map

$(BIN_DIR)/kernel.sym: $(BIN_DIR)/kernel.elf
	$(OBJCOPY64) --only-keep-debug $< $@

$(BIN_DIR)/kernel.elf: $(AUTOGEN_HEADERS) $(STATIC_MODULE_OBJS) $(STATIC_MODULE_HDRS) $(KN_OBJS) $(SOURCE_DIR)/kernel.ld
	mkdir -p $(@D)
	$(LD64) --oformat elf64-x86-64 -r -m elf_x86_64 $(LDFLAGS) -T$(SOURCE_DIR)/kernel.ld $(KN_OBJS) $(STATIC_MODULE_OBJS) -o $@

.PHONY: modules
modules: $(MODULE_TARGETS)

.PHONY: clean_config
clean_config:
	rm -rf $(CONFIG_DIR)

.PHONY: config
config: clean_config $(CONFIG_TARGETS)

$(KN_CD)/%.h: config/%.h
	mkdir -p $(@D)
	
	cp $< $@

