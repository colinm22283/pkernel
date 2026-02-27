PREFIX16=i686-elf
PREFIX32=i686-elf
PREFIX64=x86_64-elf

export CC16=$(PREFIX16)-gcc
export CC32=$(PREFIX32)-gcc
export CC64=$(PREFIX64)-gcc

export CXX16=$(PREFIX16)-g++
export CXX32=$(PREFIX32)-g++
export CXX64=$(PREFIX64)-g++

export ASM16=$(PREFIX16)-as
export ASM32=$(PREFIX32)-as
export ASM64=$(PREFIX64)-as

export LD32=$(PREFIX32)-ld
export LD64=$(PREFIX64)-ld

export OBJCOPY32=$(PREFIX32)-objcopy
export OBJCOPY64=$(PREFIX64)-objcopy

export NM64=$(PREFIX64)-nm

export CFLAGS= \
	-g \
	-c \
	-std=gnu99 \
	-ffreestanding \
	-fno-exceptions \
	-nostdlib \
	-fno-stack-protector \
	-fno-asynchronous-unwind-tables \
	-mno-red-zone \
	-Wall \
	-Wextra \
	-Wno-packed-bitfield-compat \
	-Wno-unused-parameter \
	-Werror \
	-mcmodel=large \
	-D__ARCH=x86_64

export CFLAGS16=-m16 $(CFLAGS)
export CFLAGS32=-m32 $(CFLAGS)
export CFLAGS64=-m64 -mcmodel=large $(CFLAGS)
export CXXFLAGS16=-fno-rtti $(CFLAGS16)
export CXXFLAGS32=-fno-rtti $(CFLAGS32)
export CXXFLAGS64=-fno-rtti $(CFLAGS64)
export ASMFLAGS= \
	-g

export SYSTEM_MAKE_SCRIPT=$(SOURCE_DIR)/system/x86_64
export SYSTEM_OBJ_DIR=$(OBJ_DIR)/64/system/x86_64/src

export INCLUDE_DIRS=$(SYSTEM_DIR)/include

export LDSCRIPTS=$(SYSTEM_DIR)/linker/memory.ld $(SYSTEM_DIR)/linker/global.ld

export KN_OBJS+=$(SYSTEM_OBJ_DIR)/tsr/resume_tsr_kernel.o
export KN_OBJS+=$(SYSTEM_OBJ_DIR)/tsr/resume_tsr_user.o
export KN_OBJS+=$(SYSTEM_OBJ_DIR)/tsr/store_tsr_and_yield.o

export KN_OBJS+=$(SYSTEM_OBJ_DIR)/syscall/syscall_handler.o

export KN_OBJS+=$(SYSTEM_OBJ_DIR)/interrupt/interrupt.o
export KN_OBJS+=$(SYSTEM_OBJ_DIR)/interrupt/interrupt_entries.o
export KN_OBJS+=$(SYSTEM_OBJ_DIR)/interrupt/null_handlers.o
export KN_OBJS+=$(SYSTEM_OBJ_DIR)/interrupt/exception_handlers.o
export KN_OBJS+=$(SYSTEM_OBJ_DIR)/interrupt/pic_handlers.o

export KN_OBJS+=$(SYSTEM_OBJ_DIR)/pic/pic.o

export KN_OBJS+=$(SYSTEM_OBJ_DIR)/tss/tss.o

export KN_OBJS+=$(SYSTEM_OBJ_DIR)/gdt/gdt.o

export KN_OBJS+=$(SYSTEM_OBJ_DIR)/idt/idt.o

export KN_OBJS+=$(SYSTEM_OBJ_DIR)/process/trampoline.o

export KN_OBJS+=$(SYSTEM_OBJ_DIR)/paging/init.o
export KN_OBJS+=$(SYSTEM_OBJ_DIR)/paging/bitmap.o
export KN_OBJS+=$(SYSTEM_OBJ_DIR)/paging/map_kernel.o

export KN_OBJS+=$(SYSTEM_OBJ_DIR)/entry.o
export KN_OBJS+=$(SYSTEM_OBJ_DIR)/setup.o

export KN_OBJS+=$(SYSTEM_OBJ_DIR)/panic.o
export KN_OBJS+=$(SYSTEM_OBJ_DIR)/panic_font.o
