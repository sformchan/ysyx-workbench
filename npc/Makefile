TOPNAME = top




VERILATOR = verilator
VERILATOR_CFLAGS += -g -MMD --build -cc \
				-O2 --x-assign fast --x-initial fast --noassert

BUILD_DIR = ./build
OBJ_DIR = $(BUILD_DIR)/obj_dir
BIN = $(BUILD_DIR)/$(TOPNAME)

default: $(BIN)

$(shell mkdir -p $(BUILD_DIR))


# project source
VSRCS = $(shell find $(abspath ./vsrc) -name "*.v")
CSRCS = $(shell find $(abspath ./csrc) -name "*.c" -or -name "*.cc" -or -name "*.cpp")



# rules for verilator
CXXFLAGS += -g -DTOP_NAME="\"V$(TOPNAME)\""

$(BIN): $(VSRCS) $(CSRCS)
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
