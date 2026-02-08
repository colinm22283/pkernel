STATIC_MODULES=
STATIC_MODULES+=devfs
STATIC_MODULES+=sysfs
STATIC_MODULES+=x86_pci
STATIC_MODULES+=vga_frame_buffer
STATIC_MODULES+=keyboard_pio
STATIC_MODULES+=vga_terminal
STATIC_MODULES+=x86_serial_tty
STATIC_MODULES+=disc_pio
STATIC_MODULES+=pkfs

DYNAMIC_MODULES=

export MODULE_OD_OUT=$(OBJ_DIR)/module
export STATIC_MODULE_OUT=$(BUILD_DIR)/module
export MODULE_LD_SCRIPT=$(CURDIR)/modules/module.ld
export MODULE_OD_BASE=$(OBJ_DIR)/64/modules

STATIC_MODULE_OBJS=$(foreach module, $(STATIC_MODULES), $(STATIC_MODULE_OUT)/modules/$(module)/module.o)
STATIC_MODULE_HDRS=$(STATIC_MODULE_OUT)/modules/init.h
DYNAMIC_MODULE_MODS=$(foreach module, $(DYNAMIC_MODULES), $(MODULE_DIR)/$(module).mod)

MODULE_TARGETS=$(STATIC_MODULE_OBJS) $(STATIC_MODULE_HDRS) $(DYNAMIC_MODULE_MODS)

export INCLUDE_DIRS+=$(wildcard $(foreach module, $(STATIC_MODULES), $(MOD_SOURCE_DIR)/$(module)/export))

export STATIC_MODULE_INCLUDE_DIRS=$(STATIC_MODULE_OUT)

$(STATIC_MODULE_OUT)/modules/%/module.o: .FORCE
	cd modules/$* && $(MAKE) MODULE_NAME=$* $@


$(STATIC_MODULE_OUT)/modules/%.h: modules/module.mk
	mkdir -p $(STATIC_MODULE_OUT)/modules
	
	echo "#pragma once" > $@
	echo "bool module_$*_init(void);" >> $@
	echo "bool module_$*_free(void);" >> $@

$(STATIC_MODULE_OUT)/modules/init.h: $(foreach module, $(STATIC_MODULES), $(STATIC_MODULE_OUT)/modules/$(module).h)
	mkdir -p $(STATIC_MODULE_OUT)/modules

	echo "#pragma once" > $@
	$(foreach module, $(STATIC_MODULES), echo "#include <modules/$(module).h>" >> $@;)

	echo "#include <sys/debug/print.h>" >> $@

	echo "static inline bool static_module_init(void) {" >> $@
	$(foreach module, $(STATIC_MODULES), echo 'if (!module_$(module)_init()) {debug_print("Failed to load $(module)\n"); return false; }' >> $@;)
	echo "return true;" >> $@
	echo "}" >> $@

.PHONY: $(MODULE_DIR)/%.mod
$(MODULE_DIR)/%.mod:
	cd modules/$* && $(MAKE) MODULE_NAME=$* $(MODULE_DIR)/$*.mod

.FORCE:

AUTOGEN_INCLUDE_DIRS+=$(STATIC_MODULE_INCLUDE_DIRS)
AUTOGEN_HEADERS+=$(STATIC_MODULE_OUT)/modules/init.h
