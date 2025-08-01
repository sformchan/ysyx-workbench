#include "state.h"
#include "vinit.h"


int npc_state = NPC_STOP;

extern "C" void execute()
{
	for(int i = 0; i < 2; i++)
	{
		inst = pmem_read(top->pc);
		top->clk = (contextp->time() % 2 == 0) ? 1 : 0;   //驱动系统时钟
		top->eval();
		// if(!top->clk)
		// {
		// 	printf("|0x%08X  |  0x%08X  |  0x%08X  |  0x%08X  |  0x%08X  |\n", top->pc, inst, top->gpr0, top->gpr1, top->gpr2);
		// }
		if(!top->clk)
		{
			count++;
			printf("|0x%08X  |  0x%08X  |  %08d   |\n", top->pc, inst, count);
		}
		//tfp->dump(contextp->time());
		contextp->timeInc(1);
	}
}

extern "C" void run_npc(uint64_t step)
{
	
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

	printf("|pc          |  inst        |  cycle      |\n");

	for(; step > 0; step --)
	{
		
		execute();
		if(npc_state != NPC_RUNNING) break;
	}

	switch (npc_state) {
		case NPC_RUNNING: 
			npc_state = NPC_STOP; //printf("NPC_STOP\n");
			break;
		case NPC_END: 
			printf(ANSI_FG_WHITE "npc_state = " ANSI_FG_CYAN "NPC_END.\n" ANSI_NONE);
			printf("\033[44;36mNPC\033[0m" ANSI_FG_GREEN" HIT GOOD TRAP "ANSI_NONE"at pc 0x%08x\n" , top->pc);
			break;
		
	// else
	// {
	// 	printf(ANSI_FG_WHITE "npc_state = " ANSI_FG_CYAN "NPC_ABORT.\n" ANSI_NONE);
	// 	printf("\033[44;36mNPC\033[0m" ANSI_FG_RED" HIT GOOD TRAP at pc 0x%08x\n" ANSI_NONE, top->pc);
	// }
	  }
}


///
extern "C" void init_npc(int argc, char *argv[])
{
	printf("welcome to \033[44;36mNPC\033[0m!\n");
	
	//load_sum/mem
	//load_verilog_hex("/home/leonard/Desktop/sum.hex");
	parse_args(argc, argv);
	load_img();
	init_verilator(argc, argv);
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


uint32_t gpr_val[32];
const char *gpr_name[16] = {
	"zero", "ra", "sp", "gp", "tp",
	"t0", "t1", "t2",
	"s0", "s1",
	"a0", "a1", "a2", "a3", "a4", "a5"
  };

void set_gpr(int32_t i, int32_t val)
{
	gpr_val[i] = val;
}

extern "C" void print_gpr()
{
	printf("|"ANSI_FG_GREEN "PC   "ANSI_NONE "|" ANSI_FG_GREEN"0x%08x" ANSI_NONE " |\n" , top->pc);
	for(int i = 0; i < 16; i++)
	{
		if(i == 0) printf("|%s |0x%08x |\n", gpr_name[i], gpr_val[i]);
		else printf("|%s   |0x%08x |\n", gpr_name[i] , gpr_val[i]);
	}
}

extern "C" uint32_t reg_str2val(const char *s, bool *success) {
	int num = -1;
	int i = 0;
	while(i < 32)
	{
	  if(strcmp(s, gpr_name[i]) == 0)
	  {
		num = i;
		*success = true;
		return gpr_val[num];
	  }
	  i++;
	}
	
	return num;
  }