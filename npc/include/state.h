#ifndef __STATE_H__
#define __STATE_H__


enum {NPC_STOP, NPC_RUNNING, NPC_END, NPC_QUIT, NPC_ABORT};

#include <stdio.h>
#include "memory.h"
#include "common.h"
#include "utils.h"
#include </home/leonard/ysyx-workbench/npc/include/generated/autoconf.h>
#include "difftest.h"

extern uint32_t gpr_val[];
extern const char *gpr_name[];
extern int npc_state;
extern uint32_t inst;
extern uint32_t count;
extern CPU_state cpu;
extern bool is_batch_mode;


#define ysyx_25020047_GPR_NUM 32

extern "C" void init_npc(int argc, char *argv[]);
extern "C" void execute();
extern "C" void run_npc(uint64_t step);
extern "C" void set_npc_state(int i);
extern "C" void print_gpr();
extern "C" uint32_t reg_str2val(const char *s, bool *success);


#endif
