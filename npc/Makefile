
VERILATOR = verilator
VERILATOR_CFLAGS = -Wall --cc --exe --trace --build -j 0
OBJ_DIR = obj_dir
TARGET = Vtop


all: $(OBJ_DIR)/$(TARGET)


VSRCS = $(shell find $(abspath ./vsrc) -name "*.v")
CSRCS = $(shell find $(abspath ./csrc) -name "*.c" -or -name "*.cc" -or -name "*.cpp")


$(OBJ_DIR)/$(TARGET): $(VSRCS) $(CSRCS)
	$(VERILATOR) $(VERILATOR_CFLAGS) $(VSRCS) $(CSRCS) -o $(TARGET)
		

sim: $(OBJ_DIR)/$(TARGET)
		@$^
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	
	


.PHONY:clean default all sim

clean:
	rm -rf $(OBJ_DIR)


include ../Makefile
