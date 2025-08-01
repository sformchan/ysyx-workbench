#include <stdio.h>
#include <stdlib.h>
#include "vinit.h"
#include "utils.h"
#include "memory.h"
#include "state.h"


int inst = 0;
uint32_t count = 0;
VerilatedContext* contextp = NULL;
Vtop* top = NULL;


void sdb_mainloop();

int main(int argc, char** argv)
{
	
////////initialization///////
	init_npc(argc, argv);
	//init_verilator(argc, argv);
	//top->rst = 1;
	top->clk = 1;
	top->pc = ysyx_25020047_INITADDR;
	top->rst = 0;
	
////////execution////////
	sdb_mainloop();


////////exit///////////
	delete top;
	//tfp->close();
	delete contextp;
	return ((npc_state == NPC_END) || (npc_state == NPC_QUIT)) ? 0 : 1;
		
}


