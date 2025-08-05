#include "state.h"
#include "vinit.h"
#include "utils.h"

bool check_wp();
int npc_state = NPC_STOP;
void init_monitor();
void disassemble(char *str, int size, uint64_t pc, uint8_t *code, int nbyte);
void ringbuf_push(char *log);
void ringbuf_print();
void ftrace_exec(uint32_t pc, uint32_t target, uint32_t rd, uint32_t rs1, int32_t imm);

static bool g_print_step = false;



static void trace_and_difftest(char *logbuf) {
	#ifdef CONFIG_ITRACE
	  log_write("%s\n", logbuf); 
	  if (g_print_step) puts(logbuf);
	  //IFDEF(CONFIG_DIFFTEST, difftest_step(_this->pc, dnpc));
	#endif
	#ifdef CONFIG_WATCHPOINT
	  if(check_wp())
	  {
		if(npc_state != NPC_END)
		{
		  npc_state = NPC_STOP;
		}	
	  }
	#endif 
}


extern "C" void execute()
{
	
	for(int i = 0; i < 2; i++)
	{
		top->clk = (contextp->time() % 2 == 0) ? 1 : 0;   //drive the sys_clk
		top->eval();
		//printf("%d\n", top->clk);
		inst = pmem_read(top->pc, 0);
		
		if(inst == 0xFFFFFFFF)
		{
			perror(ANSI_FG_RED "ERROR READING\n" ANSI_NONE);
			exit(1);
		}
		contextp->timeInc(1);
	} 

	//FTRACE
		#ifdef CONFIG_FTRACE
		uint32_t rd = (inst >> 7) & 0x1f;
		int32_t imm = (int32_t)(inst) >> 20;
		uint32_t rs1 = (inst >> 15) & 0x1f;
		uint32_t target = top->dnpc;
		uint32_t opcode = inst & 0x7F;
		if(opcode == 0x67) ftrace_exec(top->pc, top->dnpc, rd, rs1, imm);
		#endif

	//ITRACE
		count++;
		// printf("|0x%08X  |0x%08X  |%08d   |\n", top->pc, inst, count);
		char logbuf[128];
		#ifdef CONFIG_ITRACE
		char *p = logbuf;
		p += snprintf(p, sizeof(logbuf), "0x%08x" ":", top->pc);
		int ilen = 4;
		int i;
		uint8_t *inst_ = (uint8_t *)&inst;
		for (i = ilen - 1; i >= 0; i --) {
		  p += snprintf(p, 4, " %02x", inst_[i]);
		}
		int ilen_max = 4;
		int space_len = ilen_max - ilen;
		if (space_len < 0) space_len = 0;
		space_len = space_len * 3 + 1;
		memset(p, ' ', space_len);
		p += space_len;
		disassemble(p, logbuf + sizeof(logbuf) - p,
			top->pc, (uint8_t *)&inst, ilen);
	  	#endif
		#ifdef CONFIG_IRINGBUF
		  char destbuf[128];
		  char *r = destbuf;
		  r += snprintf(r, sizeof(destbuf) / 2, "0x%08x" ": ", top->pc);
		  //r += snprintf(r, 8, " ");
		  int rlen = 4;
		  int j;
		  uint8_t *inst_r = (uint8_t *)&inst;
		  for (j = rlen - 1; j >= 0; j --) {
			r += snprintf(r, 4, "%02x ", inst_r[j]);
		  }
		  r += snprintf(r, 8, "  ");
		
		#ifdef CONFIG_ITRACE
		  disassemble(r, destbuf + sizeof(destbuf) / 2 + 20 - r,
			  top->pc, (uint8_t *)&inst, rlen);
		#endif
		  char *dest = strdup(destbuf);
		  //printf("%s\n", destbuf);   ///test///
		  ringbuf_push(dest);
		  if(npc_state == NPC_ABORT)    // improved
		  {
			ringbuf_print();
		  }
		#endif
		trace_and_difftest(logbuf);
}

extern "C" void run_npc(uint64_t step)
{
	g_print_step = ((uint64_t)step < 10);
	switch (npc_state) {
		case NPC_END: case NPC_QUIT:
		  printf("Program execution has ended. To restart the program, exit NPC and run again.\n");
		  return;
		default: npc_state = NPC_RUNNING;
	  }


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
			printf(ANSI_FG_WHITE "npc_state " ANSI_NONE "= " ANSI_FG_CYAN "NPC_END.\n" ANSI_NONE);
			printf("\033[44;36mNPC\033[0m" ANSI_FG_GREEN " HIT GOOD TRAP " ANSI_NONE "at pc 0x%08x (%d cycle(s))\n" , top->pc, count);
			break;
		case NPC_ABORT:
			printf(ANSI_FG_WHITE "npc_state " ANSI_NONE "= " ANSI_FG_RED "NPC_ABORT.\n" ANSI_NONE);
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
	//load_verilog_hex("/home/leonard/Desktop/sum.hex");
	parse_args(argc, argv);
	load_img();
	init_verilator(argc, argv);
	init_monitor();
	printf("\033[32mStimulation starting...\033[0m\n");
}


extern "C" void set_npc_state(int state)
{
	npc_state = state;
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
	printf("|" ANSI_FG_GREEN "PC   " ANSI_NONE "|" ANSI_FG_GREEN "0x%08x" ANSI_NONE " |\n" , top->pc);
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