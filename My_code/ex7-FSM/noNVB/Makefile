VSRC = $(wildcard ./top.v) 
CSRC = $(wildcard ./sim_main.cpp)

all: 

sim: 
	verilator -Wno-fatal --cc --exe --build -j 0 -Wall $(CSRC) $(VSRC) --trace    
	./obj_dir/Vtop                


.PHONY:clean  all sim

clean:	
	rm -rf obj_dir wave.vcd


