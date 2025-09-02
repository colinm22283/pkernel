ARCH?=x86_64

ifeq ($(ARCH), x86_64)

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

export CFLAGS=-c -std=gnu99 -ffreestanding -fno-exceptions -nostdlib -fno-stack-protector -fno-asynchronous-unwind-tables -mno-red-zone -Wall -Wextra -Wno-packed-bitfield-compat -Wno-unused-parameter
export CFLAGS16=-m16 $(CFLAGS)
export CFLAGS32=-m32 $(CFLAGS)
export CFLAGS64=-m64 -mcmodel=large $(CFLAGS)
export CXXFLAGS16=-fno-rtti $(CFLAGS16)
export CXXFLAGS32=-fno-rtti $(CFLAGS32)
export CXXFLAGS64=-fno-rtti $(CFLAGS64)
export ASMFLAGS=

export SYSTEM_DIR=$(SOURCE_DIR)/system/x86_64

export INCLUDE_DIRS=$(SYSTEM_DIR)/include

export LDSCRIPTS=$(SYSTEM_DIR)/linker/memory.ld $(SYSTEM_DIR)/linker/global.ld

endif

ifeq ($(ARCH), arm64)

endif

export LDFLAGS=$(foreach d, $(LDSCRIPTS), -T$d)
