VSRC = $(wildcard ./vsrc/top.v)
CSRC = $(wildcard ./csrc/top_main.cpp)


all:
	@echo "Write this Makefile by your self."

sim:
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	@echo "Write this Makefile by your self."
	verilator -Wno-fatal --cc --exe --build -j 0 -Wall $(CSRC) $(VSRC) --trace
	./obj_dir/Vtop
	gtkwave wave.vcd

.PHONY:clean

clean:
	rm -rf obj_dir wave.vcd

include ../Makefile
