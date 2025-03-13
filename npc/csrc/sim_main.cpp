#include <stdio.h>
#include <stdlib.h>
#include "Vnpc_top.h"
#include "verilated.h"
#include "verilated_fst_c.h"

int main(int argc, char** argv)
{
	VerilatedContext* contextp = new VerilatedContext;
	contextp->commandArgs(argc, argv);
	Vnpc_top* top = new Vnpc_top{contextp};


	VerilatedFstC* tfp = new VerilatedFstC;
	contextp->traceEverOn(true);
	top->trace(tfp,0);
	tfp->open("wave.fst");

	while(contextp->time() <= 5)
	{
        top->inst = pmem(top->pc);
		top->eval();
		tfp->dump(contextp->time());
		contextp->timeInc(1);

	}
	delete top;
	tfp->close();
	delete contextp;
	return 0;	
}