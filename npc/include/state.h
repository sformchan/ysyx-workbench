#ifndef __STATE_H__
#define __STATE_H__


enum {NPC_STOP, NPC_RUNNING, NPC_END, NPC_QUIT, NPC_ABORT};

#include <stdio.h>
#include "memory.h"
#include "common.h"
#include "utils.h"
#include </home/leonard/ysyx-workbench/npc/include/generated/autoconf.h>
#include "difftest.h"

typedef uint32_t word_t;
typedef uint32_t vaddr_t;

typedef struct {
	word_t mtvec;
	word_t mepc;
	word_t mstatus;
	word_t mcause;
} CSR;

typedef struct {
	word_t gpr[32];   // 32 个通用寄存器    // fuckin amount of gpr!!!!!
	vaddr_t pc;       // 程序计数器
	CSR csr;
  } CPU_state;
  
extern uint32_t gpr_val[];
extern const char *gpr_name[];
extern int npc_state;
extern uint32_t inst;
extern uint32_t count;
extern CPU_state cpu;
extern bool is_batch_mode;
extern long img_size;

#define ysyx_25020047_GPR_NUM 32

extern "C" void init_npc(int argc, char *argv[]);
extern "C" void execute();
extern "C" void run_npc(uint64_t step);
extern "C" void set_npc_state(int i);
extern "C" void print_gpr();
extern "C" uint32_t reg_str2val(const char *s, bool *success);


#endif
