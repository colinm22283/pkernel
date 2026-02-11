INCLUDE_PARAMS=$(foreach d, $(INCLUDE_DIRS) $(AUTOGEN_INCLUDE_DIRS), -I$d)
CHEADERS=$(foreach d, $(INCLUDE_DIRS), $(shell find $d -type f -name '*.h'))

OPTIMIZATION?=3

$(OBJ_DIR)/64/%.o: $(SOURCE_DIR)/%.c $(CHEADERS) $(AUTOGEN_HEADERS)
	mkdir -p $(@D)
	$(CC64) $(CFLAGS64) -O$(OPTIMIZATION) $(INCLUDE_PARAMS) -mgeneral-regs-only $< -o $@

$(OBJ_DIR)/64/%.o: $(SOURCE_DIR)/%.s
	mkdir -p $(@D)
	$(ASM64) $(ASMFLAGS) $< -o $@
