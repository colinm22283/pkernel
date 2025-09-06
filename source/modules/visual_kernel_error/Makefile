MODULE_SOURCE_DIR=$(CURDIR)/src
MODULE_OD_64=$(MODULE_OD_BASE)/$(MODULE_NAME)/src

include sources.mk

include $(MAKE_SCRIPTS)

$(MODULE_OD_OUT)/$(MODULE_NAME).o: $(MODULE_OBJS)
	mkdir -p $(MODULE_OD_OUT)

	$(LD64) -r $(MODULE_OBJS) -o $(MODULE_OD_OUT)/$(MODULE_NAME).o

$(STATIC_MODULE_OUT)/modules/$(MODULE_NAME)/module.o: $(MODULE_OD_OUT)/$(MODULE_NAME).o
	mkdir -p $(STATIC_MODULE_OUT)/modules/$(MODULE_NAME)

	$(OBJCOPY64) \
		--rename-section .text=.module_text \
		--rename-section .data=.module_data \
		--rename-section .rodata=.module_rodata \
		--rename-section .bss=.module_bss \
		$$( \
			for s in `nm $(MODULE_OD_OUT)/$(MODULE_NAME).o --format=just-symbols --extern-only -U`; do \
				echo --redefine-sym $$s="module_$(MODULE_NAME)_$$s"; \
			done \
		) $(MODULE_OD_OUT)/$(MODULE_NAME).o $@

$(MODULE_DIR)/$(MODULE_NAME).mod: $(MODULE_OD_OUT)/$(MODULE_NAME).o
	$(MKMOD_BIN) $(MODULE_OD_OUT)/$(MODULE_NAME).o $@
	mkdir -p $(MODULE_DIR)
	echo test > $(MODULE_DIR)/$(MODULE_NAME).mod
