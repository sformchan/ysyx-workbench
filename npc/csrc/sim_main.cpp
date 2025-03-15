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

	while(contextp->time() <= 10)
	{
		top->rst = 0;
		top->clk = (contextp->time() % 2 == 0) ? 1 : 0;
		top->wen = wen;
        top->inst = read_inst(top->pc);
		top->eval();
		//tfp->dump(contextp->time());
		contextp->timeInc(1);
		printf("pc:   %u\n", top->pc);
		printf("inst: %u\n", top->inst);

	}
	delete top;
	//tfp->close();
	delete contextp;
	return 0;	
}