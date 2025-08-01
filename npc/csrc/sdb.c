#include <stdio.h>
#include "utils.h"
#include "state.h"
#include <string.h>
#include "vinit.h"


char *readline_(const char *prompt);

static char* rl_gets() {
	static char *line_read = NULL;
  
	if (line_read) {
	  free(line_read);
	  line_read = NULL;
	}
  
	line_read = readline_(ANSI_FG_CYAN "(npc) " ANSI_NONE);
  
	// if (line_read && *line_read) {
	//   add_history(line_read);
	// }
  
	return line_read;
}


static int cmd_help(char *args);

static int cmd_c(char *args) {
	run_npc(-1);
	return 0;
}

static int cmd_si(char *args)
{
  int step;
  if(args == NULL)
  {
    step = 1;
  }
  else
  {
    int result = sscanf(args, "%d", &step);
    if(result != 1 || step < 1)
    {
      step = 1;
      printf("already put step as 1. pls prompt a valid integer\n");
    } 
  }
  run_npc(step);
  return 0;
}

static int cmd_q(char *args) {
	npc_state = NPC_QUIT;
	return -1;
}

static struct {
	const char *name;
	const char *description;
	int (*handler) (char *);
  } cmd_table [] = {
	{"help" , ANSI_FG_CYAN "Display information about all supported commands" ANSI_NONE , cmd_help },
	{"c", "   Continue the execution of the program", cmd_c },
	{"q", ANSI_FG_CYAN "   Exit NPC" ANSI_NONE , cmd_q },
	{"si", "  Let the program step, you can enter a number after 'si'", cmd_si },
	//{ "info", ANSI_FG_CYAN "Print info of reg or wp" ANSI_NONE, cmd_info },
	//{ "x", "   Visit the target memory and print it, you are expected to enter an expression", cmd_x},
	//{ "p", ANSI_FG_CYAN "   Calculate the result of the given expression" ANSI_NONE, cmd_p},
	//{ "w", "   Set a new watchpoint to monitor the given expression", cmd_w},
	//{ "d", ANSI_FG_CYAN "   Delete the watchpoint with sequence number 'n'" ANSI_NONE, cmd_d},
	//{ "ext", "   Test your expr_function", cmd_t}
  
	/* TODO: Add more commands */
  
  }; 


#define ARRLEN(arr) (int)(sizeof(arr) / sizeof(arr[0]))
#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;
  
	if (arg == NULL) {
	  /* no argument given */
	  for (i = 0; i < NR_CMD; i ++) {
		printf("%s  %s\n", cmd_table[i].name, cmd_table[i].description);
	  }
	}
	else {
	  for (i = 0; i < NR_CMD; i ++) {
		if (strcmp(arg, cmd_table[i].name) == 0) {
		  printf("%s  %s\n", cmd_table[i].name, cmd_table[i].description);
		  return 0;
		}
	  }
	  printf("Unknown command '%s'\n", arg);
	}
	return 0;
  }

void sdb_mainloop() {  
	for (char *str; (str = rl_gets()) != NULL; ) {
	  char *str_end = str + strlen(str);
  
	  /* extract the first token as the command */
	  char *cmd = strtok(str, " ");
	  if (cmd == NULL) { continue; }
  
	  /* treat the remaining string as the arguments,
	   * which may need further parsing
	   */
	  char *args = cmd + strlen(cmd) + 1;
	  if (args >= str_end) {
		args = NULL;
	  }
	  int i;
	  for (i = 0; i < NR_CMD; i ++) {
		if (strcmp(cmd, cmd_table[i].name) == 0) {
		  if (cmd_table[i].handler(args) < 0) { return; }   
		  break;
		}
	  }
  
	  if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
  }