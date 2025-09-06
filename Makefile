export BUILD_DIR?=$(CURDIR)/build
export BIN_DIR?=$(BUILD_DIR)/bin
export MODULE_DIR?=$(BIN_DIR)/module
export OBJ_DIR?=$(BUILD_DIR)/obj
export SOURCE_DIR?=$(CURDIR)/source

export MAKE_DIR=$(CURDIR)/make
export MAKE_SCRIPTS=$(MAKE_DIR)/targets16.mk $(MAKE_DIR)/targets32.mk $(MAKE_DIR)/targets64.mk

include $(MAKE_DIR)/arch.mk
include $(MAKE_SCRIPTS)

.DEFAULT: all
.PHONY: all
all: kernel

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

.PHONY: kernel
kernel:
	cd source && $(MAKE) kernel

.PHONY: modules
modules:
	cd source && $(MAKE) modules

.PHONY: external
external:
	cd $(EXTERNAL_DIR) && $(MAKE)

.FORCE:
