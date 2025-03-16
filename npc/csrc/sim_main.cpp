#include <stdio.h>
#include <stdlib.h>
#include "rom.h"
#include "Vtop.h"
#include "verilated.h"
#include "svdpi.h"
#include "Vtop__Dpi.h"
//#include "verilated_fst_c.h"

int stop = 0;
void stop_stimulation()
{
	printf("\033[33mSimulation stopped successfully due to ebreak!\033[0m\n");
	stop = 1;
}

int main(int argc, char** argv)
{
	printf("welcome to \033[44;36mNPC\033[0m!\n");
	printf("\033[32mStimulation starting...\033[0m\n");
	VerilatedContext* contextp = new VerilatedContext;
	contextp->commandArgs(argc, argv);
	Vtop* top = new Vtop{contextp};


	//VerilatedFstC* tfp = new VerilatedFstC;
	contextp->traceEverOn(true);
	//top->trace(tfp,0);
	//tfp->open("wave.fst");
	top->rst = 1;
	top->pc = ysyx_25020047_INITADDR;
	top->clk = 0;

    printf("|pc          |  inst        |  gpr0        |  gpr1        |  gpr2        |\n");
	while(!stop)
	{	
			
		top->rst = 0;
		top->inst = read_inst(top->pc);
		top->clk = (contextp->time() % 2 == 0) ? 1 : 0;   //驱动系统时钟
        
		top->eval();
		if(top->clk)
		{
			printf("|0x%08X  |  0x%08X  |  0x%08X  |  0x%08X  |  0x%08X  |\n", top->pc, top->inst, top->gpr0, top->gpr1, top->gpr2);
		}
		//tfp->dump(contextp->time());
		contextp->timeInc(1);
		
	}
	delete top;
	//tfp->close();
	delete contextp;
	return 0;	
}