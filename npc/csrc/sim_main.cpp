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
	for(int i = 0; i < 2; i++)
	{
		
		
		top->clk = (contextp->time() % 2 == 0) ? 1 : 0;   //驱动系统时钟
		
		
		top->eval();
		// if(!top->clk)
		// {
		// 	printf("|0x%08X  |  0x%08X  |  0x%08X  |  0x%08X  |  0x%08X  |\n", top->pc, inst, top->gpr0, top->gpr1, top->gpr2);
		// }
		//printf("%d\n", top->clk);
		inst = pmem_read(top->pc);
		// if(!top->clk)
		// {
		// 	count++;
		// 	printf("|0x%08X  |0x%08X  |%08d   |\n", top->pc, inst, count);
		// }
		//tfp->dump(contextp->time());
		contextp->timeInc(1);
	}
	sdb_mainloop();


////////exit///////////
	delete top;
	//tfp->close();
	delete contextp;
	return ((npc_state == NPC_END) || (npc_state == NPC_QUIT)) ? 0 : 1;
		
}


