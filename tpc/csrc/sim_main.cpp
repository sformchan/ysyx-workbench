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
	stop = 1;
}

uint32_t read_inst(uint32_t pc) {
    // 检查地址是否对齐到 4 字节
    if (pc % 4 != 0) {
        printf("Error: PC address 0x%08x is not aligned to 4 bytes.\n", pc);
        return 0xFFFFFFFF; // 返回错误码
    }

    // 检查地址是否在合法范围内
    if (pc < ysyx_25020047_INITADDR || pc >= ysyx_25020047_INITADDR + ysyx_25020047_SIZE * 4) {
        printf("Error: PC address 0x%08x is out of ROM range.\n", pc);
        return 0xFFFFFFFF; // 返回错误码
    }

    // 计算数组索引
    uint32_t real_addr = (pc - ysyx_25020047_INITADDR) / 4;

    // 返回指令
    return rom[real_addr];
}

int main(int argc, char** argv)
{
	printf("welcome to \033[44;36mTPC\033[0m!\n");
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
	u_int32_t inst = 0;
    printf("|pc          |  inst        |  gpr0        |  gpr1        |  gpr2        |\n");
	while(!stop)
	{	
			
		top->rst = 0;
		
		top->clk = (contextp->time() % 2 == 0) ? 1 : 0;   //驱动系统时钟
        inst = read_inst(top->pc);
		top->eval();
		if(!top->clk)
		{
			printf("|0x%08X  |  0x%08X  |  0x%08X  |  0x%08X  |  0x%08X  |\n", top->pc, inst, top->gpr0, top->gpr1, top->gpr2);
		}
		//tfp->dump(contextp->time());
		contextp->timeInc(1);
		
	}
	if(stop)
	{
		printf("\033[33mSimulation stopped successfully due to ebreak!\033[0m\n");
	}
	delete top;
	//tfp->close();
	delete contextp;
	return 0;	
}