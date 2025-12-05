export BUILD_DIR?=$(CURDIR)/build
export BIN_DIR?=$(BUILD_DIR)/bin
export MODULE_DIR?=$(BIN_DIR)/module
export OBJ_DIR?=$(BUILD_DIR)/obj
export SOURCE_DIR?=$(CURDIR)/source
export MOD_SOURCE_DIR?=$(SOURCE_DIR)/modules

export MAKE_DIR=$(CURDIR)/make
export MAKE_SCRIPTS=$(MAKE_DIR)/targets16.mk $(MAKE_DIR)/targets32.mk $(MAKE_DIR)/targets64.mk

include $(MAKE_DIR)/arch.mk
include $(MAKE_SCRIPTS)

.DEFAULT: all
.PHONY: all
all:
	cd source && $(MAKE) all

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

.PHONY: kernel
kernel:
	cd source && $(MAKE) kernel

.PHONY: modules
modules:
	cd source && $(MAKE) modules

.PHONY: application
application: $(OBJ_DIR)/application_start_table.o
	if [[ ! -v APPLICATION_DIR ]]; then echo "ERROR: APPLICATION_DIR varaible not set!"; false; fi
	
	cd $(APPLICATION_DIR) && $(MAKE) \
		AS="$(ASM64)" \
		ASFLAGS="$(ASMFLAGS64)" \
		CC="$(CC64)" \
		CFLAGS="$(CFLAGS64)" \
		LD="$(LD64)" \
		LDFLAGS="$(LDFLAGS) -T$(CURDIR)/application/application.ld $(OBJ_DIR)/application_start_table.o" \
		INCLUDE_DIRS="$(SOURCE_DIR)/shared/include"

$(OBJ_DIR)/application_start_table.o: $(CURDIR)/application/application_start_table.c
	mkdir -p $(@D)
	$(CC64) $(CFLAGS64) -I$(SOURCE_DIR)/shared/include $< -o $@

.FORCE:

