#ifndef __STATE_H__
#define __STATE_H__

enum {NPC_STOP, NPC_RUNNING, NPC_END, NPC_QUIT};

extern int npc_state = NPC_STOP;


void end_stimulation()
{
	npc_state = NPC_END;
}


#endif