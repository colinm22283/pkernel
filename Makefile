export BUILD_DIR?=$(CURDIR)/build
export BIN_DIR?=$(BUILD_DIR)/bin
export MODULE_DIR?=$(BIN_DIR)/module
export OBJ_DIR?=$(BUILD_DIR)/obj
export SOURCE_DIR?=$(CURDIR)/source

export MAKE_DIR=$(CURDIR)/make
export MAKE_SCRIPTS=$(MAKE_DIR)/targets16.mk $(MAKE_DIR)/targets32.mk $(MAKE_DIR)/targets64.mk

include $(MAKE_DIR)/arch.mk
include $(MAKE_SCRIPTS)

.PHONY: kernel
kernel: $(BIN_DIR)/kernel.bin

.PHONY: $(BIN_DIR)/kernel.bin
$(BIN_DIR)/kernel.bin:
	cd source && $(MAKE) $(BIN_DIR)/kernel.bin

.PHONY: modules
modules:
	cd source && $(MAKE) modules

.PHONY: external
external:
	cd $(EXTERNAL_DIR) && $(MAKE)

.PHONY: all
all: kernel

.DEFAULT: all
