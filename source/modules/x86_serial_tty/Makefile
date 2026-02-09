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
		--redefine-sym init=module_$(MODULE_NAME)_init \
		--redefine-sym free=module_$(MODULE_NAME)_free \
		$$( \
			$(NM64) $(MODULE_OD_OUT)/$(MODULE_NAME).o --format=sysv --extern-only -U | tail -n +7 | while IFS= read -r s; do \
				IFS="|" set -- $$s; \
				if [ "$$7" != ".module_export" ]; then \
				  	if [ "$$1" != "init" ] && [ "$$1" != "free" ]; then \
						echo --localize-symbol=$$1; \
					fi; \
				fi; \
			done \
		) \
		$(MODULE_OD_OUT)/$(MODULE_NAME).o $@

$(MODULE_DIR)/$(MODULE_NAME).mod: $(MODULE_OD_OUT)/$(MODULE_NAME).o
	$(MKMOD_BIN) $(MODULE_OD_OUT)/$(MODULE_NAME).o $@
	mkdir -p $(MODULE_DIR)
	echo test > $(MODULE_DIR)/$(MODULE_NAME).mod

$(CONFIG_DIR)/modules/$(MODULE_NAME)/config.h: config.h
	mkdir -p $(@D)

	cp $< $@
