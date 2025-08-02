TOPNAME = top




VERILATOR = verilator
VERILATOR_CFLAGS += -MMD --build -cc \
				-O2 --x-assign fast --x-initial fast --noassert

BUILD_DIR = ./build
OBJ_DIR = $(BUILD_DIR)/obj_dir
BIN = $(BUILD_DIR)/$(TOPNAME)

default: $(BIN)

$(shell mkdir -p $(BUILD_DIR))


# project source
# VSRCS = $(shell find $(abspath ./vsrc) -name "*.v")
# CSRCS = $(shell find $(abspath ./csrc) -name "*.c" -or -name "*.cc" -or -name "*.cpp")
VSRCS = $(shell find vsrc -type f \( -name "*.v" -o -name "*.sv" \))
CSRCS = $(shell find csrc -type f \( -name "*.c" -o -name "*.cc" -o -name "*.cpp" \))



# rules for verilator
CXXFLAGS += -DTOP_NAME="\"V$(TOPNAME)\""
# 添加配置头文件


$(BIN): $(VSRCS) $(CSRCS) $(NPC_HOME)/include/generated/autoconf.h
	@rm -rf $(OBJ_DIR)
	$(VERILATOR) $(VERILATOR_CFLAGS) \
		--top-module $(TOPNAME) \
		$(VSRCS) \
		$(addprefix -CFLAGS , $(CXXFLAGS)) \
		--Mdir $(OBJ_DIR) --exe $(CSRCS) \
		-o $(abspath $(BIN))

all: default
		
IMG ?=
run: $(BIN)
	@$(BIN) $(IMG)
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	
gdb: $(BIN)
	@$(call git_commit, "sim RTL")
	@gdb --args $(BIN) $(IMG)



clean:
	rm -rf $(BUILD_DIR)

.PHONY: default all clean run


include ../Makefile
