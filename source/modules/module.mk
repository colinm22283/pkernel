STATIC_MODULES=
STATIC_MODULES+=vga_frame_buffer
STATIC_MODULES+=keyboard_pio
STATIC_MODULES+=vga_terminal
#STATIC_MODULES+=visual_kernel_error
#STATIC_MODULES+=devmem
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

export STATIC_MODULE_INCLUDE_DIRS=$(STATIC_MODULE_OUT)

$(STATIC_MODULE_OUT)/modules/%/module.o: .FORCE
	cd modules/$* && $(MAKE) MODULE_NAME=$* $(MODULE_OD_OUT)/$*.o

	mkdir -p $(STATIC_MODULE_OUT)/modules/$*

	rm -f $@
	$(OBJCOPY64) --rename-section .text=.module_text --rename-section .data=.module_data --rename-section .rodata=.module_rodata --rename-section .bss=.module_bss $$(for s in `nm $(MODULE_OD_OUT)/$*.o --format=just-symbols --extern-only -U`; do echo --redefine-sym $$s="module_$*_$$s"; done) $(MODULE_OD_OUT)/$*.o $@
	sync

$(STATIC_MODULE_OUT)/modules/%.h: modules/module.mk
	mkdir -p $(STATIC_MODULE_OUT)/modules

	echo "#pragma once" > $@
	echo "bool module_$*_init(void);" >> $@
	echo "bool module_$*_free(void);" >> $@

.PHONY: $(MODULE_DIR)/%.mod
$(MODULE_DIR)/%.mod:
	cd modules/$* && $(MAKE) MODULE_NAME=$* $(MODULE_DIR)/$*.mod

$(STATIC_MODULE_OUT)/modules/init.h: $(foreach module, $(STATIC_MODULES), $(STATIC_MODULE_OUT)/modules/$(module).h)
	mkdir -p $(STATIC_MODULE_OUT)/modules

	echo "#pragma once" > $@
	$(foreach module, $(STATIC_MODULES), echo "#include <modules/$(module).h>" >> $@;)

	echo "static inline bool static_module_init(void) {" >> $@
	$(foreach module, $(STATIC_MODULES), echo "if (!module_$(module)_init()) return false;" >> $@;)
	echo "return true;" >> $@
	echo "}" >> $@

.FORCE:
