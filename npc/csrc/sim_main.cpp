#include <stdio.h>
#include <stdlib.h>
#include "Vtop.h"
#include "verilated.h"
#include "svdpi.h"
#include "Vtop__Dpi.h"
#include "utils.h"
#include "memory.h"
#include "state.h"
//#include "verilated_fst_c.h"


int main(int argc, char** argv)
{
	printf("welcome to \033[44;36mNPC\033[0m!\n");
	
	//load_sum/mem
	//load_verilog_hex("/home/leonard/Desktop/sum.hex");
	parse_args(argc, argv);
	load_img();

	printf("\033[32mStimulation starting...\033[0m\n");

	VerilatedContext* contextp = new VerilatedContext;
	contextp->commandArgs(argc, argv);
	Vtop* top = new Vtop{contextp};
	//VerilatedFstC* tfp = new VerilatedFstC;
	contextp->traceEverOn(true);
	//top->trace(tfp,0);
	//tfp->open("wave.fst");
	top->rst = 1;
	top->clk = 0;
	top->pc = ysyx_25020047_INITADDR;
	int inst = 0;
	uint32_t count = 0;
    //printf("|pc          |  inst        |  gpr0        |  gpr1        |  gpr2        |\n");
	//printf("|pc          |  inst        |  cycle      |\n");
	npc_state = NPC_RUNNING;
	while(npc_state == NPC_RUNNING)
	{	
			
		top->rst = 0;
		inst = pmem_read(top->pc);
		if(inst == 0xFFFFFFFF)
		{
			perror(ANSI_FG_RED"ERROR READING\n" ANSI_NONE);
			exit(1);
		}
		top->clk = (contextp->time() % 2 == 0) ? 1 : 0;   //驱动系统时钟
		top->eval();
		// if(!top->clk)
		// {
		// 	printf("|0x%08X  |  0x%08X  |  0x%08X  |  0x%08X  |  0x%08X  |\n", top->pc, inst, top->gpr0, top->gpr1, top->gpr2);
		// }
		if(!top->clk)
		{
			count++;
		}
		//tfp->dump(contextp->time());
		contextp->timeInc(1);
		
	}
	inst = pmem_read(top->pc);
	//printf("|0x%08X  |  0x%08X  |  %08d   |\n", top->pc, inst, count);
	if(npc_state == NPC_END)
	{
		//printf("\033[33mSimulation stopped successfully due to ebreak!\033[0m\n");
		printf("\033[44;36mNPC\033[0m" ANSI_FG_GREEN" HIT GOOD TRAP at pc 0x%08x\n" ANSI_NONE, top->pc);
	}
	else printf("\033[44;36mNPC\033[0m" ANSI_FG_RED" HIT GOOD TRAP at pc 0x%08x\n" ANSI_NONE, top->pc);
	delete top;
	//tfp->close();
	delete contextp;
	return 0;	
}