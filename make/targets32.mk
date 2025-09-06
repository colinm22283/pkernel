INCLUDE_PARAMS=$(foreach d, $(INCLUDE_DIRS) $(AUTOGEN_INCLUDE_DIRS), -I$d)
CHEADERS=$(foreach d, $(INCLUDE_DIRS), $(shell find $d -type f -name '*.h'))
CXXHEADERS=$(foreach d, $(INCLUDE_DIRS), $(shell find $d -type f -name '*.hpp'))

OPTIMIZATION?=3

$(OBJ_DIR)/32/%.o: $(SOURCE_DIR)/%.c $(CHEADERS) $(AUTOGEN_HEADERS)
	mkdir -p $(@D)
	$(CC32) $(CFLAGS32) -O$(OPTIMIZATION) $(INCLUDE_PARAMS) $< -o $@

$(OBJ_DIR)/32/%.o: $(SOURCE_DIR)/%.cpp $(CHEADERS) $(CXXHEADERS) $(AUTOGEN_HEADERS)
	mkdir -p $(@D)
	$(CXX32) $(CXXFLAGS32) -O$(OPTIMIZATION) $(INCLUDE_PARAMS) $< -o $@

$(OBJ_DIR)/32/%.o: $(SOURCE_DIR)/%.s
	mkdir -p $(@D)
	$(ASM32) $(ASMFLAGS) $< -o $@
