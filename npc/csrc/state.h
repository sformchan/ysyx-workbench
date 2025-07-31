#ifndef __STATE_H__
#define __STATE_H__


enum {NPC_STOP, NPC_RUNNING, NPC_END, NPC_QUIT};

#include <stdio.h>
#include "memory.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include "utils.h"



extern int npc_state = NPC_STOP;

// void end_stimulation();

// void init_monitor(int argc, char *argv[]);
void init_monitor(int argc, char *argv[])
{
	printf("welcome to \033[44;36mNPC\033[0m!\n");
	
	//load_sum/mem
	//load_verilog_hex("/home/leonard/Desktop/sum.hex");
	parse_args(argc, argv);
	load_img();

	printf("\033[32mStimulation starting...\033[0m\n");
}


void end_stimulation()
{
	npc_state = NPC_END;
}

#endif