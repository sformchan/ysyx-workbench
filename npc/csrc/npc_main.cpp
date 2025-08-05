#include "vinit.h"
#include "utils.h"
#include "memory.h"
#include "state.h"



VerilatedContext* contextp = NULL;
Vtop* top = NULL;
uint32_t count = 0;
uint32_t inst = 0;
	
void sdb_mainloop();
void npc_clk_once();

int main(int argc, char** argv)
{
	
////////initialization///////
	init_npc(argc, argv);
	//init_verilator(argc, argv);
	top->rst = 1;
	top->pc = ysyx_25020047_INITADDR;
	top->rst = 0;
	top->clk = 0;


////////execution////////
	sdb_mainloop();


////////exit///////////
	delete top;
	//tfp->close();
	delete contextp;
	return ((npc_state == NPC_END) || (npc_state == NPC_QUIT)) ? 0 : 1;
		
}


