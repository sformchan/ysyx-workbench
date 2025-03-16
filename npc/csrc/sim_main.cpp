#include <stdio.h>
#include <stdlib.h>
#include "rom.h"
#include "Vtop.h"
#include "verilated.h"
//#include "verilated_fst_c.h"

int main(int argc, char** argv)
{
	VerilatedContext* contextp = new VerilatedContext;
	contextp->commandArgs(argc, argv);
	Vtop* top = new Vtop{contextp};


	//VerilatedFstC* tfp = new VerilatedFstC;
	contextp->traceEverOn(true);
	//top->trace(tfp,0);
	//tfp->open("wave.fst");
	int wen = 1;
	top->rst = 1;
	top->pc = INITADDR;
    printf("|pc          |  inst        |  gpr0        |  gpr1        |  gpr2        |\n");
	while(contextp->time() <= 10)
	{
		top->rst = 0;
		top->clk = (contextp->time() % 2 == 0) ? 1 : 0;
		top->wen = wen;
        top->inst = read_inst(top->pc);
		top->eval();
		//tfp->dump(contextp->time());
		contextp->timeInc(1);
		
		printf("|0x%x  |  0x%x  |  0x%x  |  0x%x  |  0x%x  |\n", top->pc, top->inst, top->gpr0, top->gpr1, top->gpr2);
		

	}
	delete top;
	//tfp->close();
	delete contextp;
	return 0;	
}