#include "state.h"
#include "vinit.h"


int npc_state = NPC_STOP;

extern "C" void execute()
{
	for(int i = 0; i < 2; i++)
	{
		top->clk = (contextp->time() % 2 == 0) ? 1 : 0;   //驱动系统时钟
		top->eval();
		// if(!top->clk)
		// {
		// 	printf("|0x%08X  |  0x%08X  |  0x%08X  |  0x%08X  |  0x%08X  |\n", top->pc, inst, top->gpr0, top->gpr1, top->gpr2);
		// }
		if(!top->clk)
		{
			count++;
			//printf("|0x%08X  |  0x%08X  |  %08d   |\n", top->pc, inst, count);
		}
		//tfp->dump(contextp->time());
		contextp->timeInc(1);
	}
}

extern "C" void run_npc(uint64_t step)
{
	inst = pmem_read(top->pc);
		if(inst == 0xFFFFFFFF)
		{
			perror(ANSI_FG_RED"ERROR READING\n" ANSI_NONE);
			exit(1);
		}

	switch (npc_state) {
		case NPC_END: case NPC_QUIT:
		  printf("Program execution has ended. To restart the program, exit NPC and run again.\n");
		  return;
		default: npc_state = NPC_RUNNING;
	  }

	for(; step > 0; step --)
	{
		//printf("|pc          |  inst        |  cycle      |\n");
		execute();
		if(npc_state != NPC_RUNNING) break;
	}

	switch (npc_state) {
		case NPC_RUNNING: npc_state = NPC_STOP; //printf("NPC_STOP\n");
		case NPC_END: 
		{
			printf(ANSI_FG_WHITE "npc_state = " ANSI_FG_CYAN "NPC_END.\n" ANSI_NONE);
			printf("\033[44;36mNPC\033[0m" ANSI_FG_GREEN" HIT GOOD TRAP at pc 0x%08x\n" ANSI_NONE, top->pc);
		}
		
	// else
	// {
	// 	printf(ANSI_FG_WHITE "npc_state = " ANSI_FG_CYAN "NPC_ABORT.\n" ANSI_NONE);
	// 	printf("\033[44;36mNPC\033[0m" ANSI_FG_RED" HIT GOOD TRAP at pc 0x%08x\n" ANSI_NONE, top->pc);
	// }
	  }
}


extern "C" void init_npc(int argc, char *argv[])
{
	printf("welcome to \033[44;36mNPC\033[0m!\n");
	
	//load_sum/mem
	//load_verilog_hex("/home/leonard/Desktop/sum.hex");
	parse_args(argc, argv);
	load_img();

	printf("\033[32mStimulation starting...\033[0m\n");
}


extern "C" void end_npc()
{
	npc_state = NPC_END;
}

void init_verilator(int argc, char **argv) {
    contextp = new VerilatedContext;
    contextp->commandArgs(argc, argv);

    top = new Vtop{contextp};

    contextp->traceEverOn(true);
}
