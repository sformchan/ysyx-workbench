#ifndef __STATE_H__
#define __STATE_H__


enum {NPC_STOP, NPC_RUNNING, NPC_END, NPC_QUIT};

#include <stdio.h>
#include "memory.h"
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include "utils.h"

extern uint32_t gpr_val[];
extern const char *gpr_name[];
extern int npc_state;

extern int inst;
extern uint32_t count;
// void end_stimulation();

// void init_monitor(int argc, char *argv[]);


extern "C" void init_npc(int argc, char *argv[]);
extern "C" void execute();
extern "C" void run_npc(uint64_t step);
extern "C" void end_npc();
void init_verilator(int argc, char **argv);
extern "C" void print_gpr();
extern "C" uint32_t reg_str2val(const char *s, bool *success);


#endif