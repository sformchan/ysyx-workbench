#include <stdio.h>
#include "utils.h"
#include "state.h"
#include <string.h>
#include "vinit.h"
#include "memory.h"
#include "macro.h"	

uint32_t expr(char *e, bool *success);
char *readline_(const char *prompt);
void init_regex();
void init_wp_pool();
void delete_wp(int num);
void display_wp();
void set_wp(char *expr_str);


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

static int cmd_info(char *args)
{
  char letter;
  if(args == NULL)
  {
    printf(ANSI_FG_RED "ERROR" ANSI_NONE ": try entering 'w' or 'r'.\n");
  }
  else
  {
    sscanf(args, "%c", &letter);
    if(letter == 'r')
    {
      print_gpr();
    }
    else if(letter == 'w')
    {
      display_wp();
    }
  }
  return 0;
}


static int cmd_x(char *args)
{
  int length;
  char start[256];
  bool success = false;
  if(args == NULL) printf(ANSI_FG_RED "ERROR" ANSI_NONE ": NO ARGUMENT.\n");
  else
  {
	int flag = 1;
    sscanf(args, "%d %s", &length, start);
    uint32_t result = expr(start, &success);
    //printf("%x\n", result);
	if(length < 1) printf(ANSI_FG_RED "ERROR" ANSI_NONE ": INVALID LENGTH: %d\n", length), flag = 0;
	if(result < 0x80000000 || result > 0x87ffffff) printf(ANSI_FG_RED "ERROR" ANSI_NONE ": INVALID MEMORY: 0x%08x\n", result), flag = 0;
    if(flag)
    {
      for(int i = 0; i < length; i++)
      {
        printf("%d 0x%08x 0x%08x\n", i, result + (i * 4), pmem_read(result + (i * 4), 1));
      }
    }
  }
  return 0;
}


static int cmd_p(char *args)
{
  if(args == NULL)
  {
    printf(ANSI_FG_RED "ERROR" ANSI_NONE ": nothing output cause of INVALID INPUT.\n");
    return 0;
  }
  bool sign = false;
 
  printf(ANSI_FG_WHITE "result" ANSI_NONE "  0x%x\n", expr(args, &sign));

  return 0;
}


static int cmd_w(char *args)
{
  if(args == NULL)
  {
    printf(ANSI_FG_RED "ERROR" ANSI_NONE ": nothing output cause of INVALID INPUT.\n");
    return 0;
  }
  set_wp(args);
  return 0;
}



static int cmd_d(char *args)
{
  
  if(args == NULL)
  {
    printf(ANSI_FG_RED "ERROR" ANSI_NONE ": nothing output cause of INVALID INPUT.\n");
    return 0;
  }
  int num = atoi(args);
  delete_wp(num);
  return 0;
}
  

static struct {
	const char *name;
	const char *description;
	int (*handler) (char *);
  } cmd_table [] = {
	{"help" , "Display information about all supported commands"                   , cmd_help },
	{"c"    , "   Continue the execution of the program"                           ,    cmd_c },
	{"q"    , "   Exit NPC"                                                        ,    cmd_q },
	{"si"   , "  Execute for steps, you can enter a number after 'si', like si 1"  ,   cmd_si },
	{"info" , "Print info of reg or wp"                                            , cmd_info },
	{"x"    , "   Visit the target memory and print it, you are expected to enter an expression", cmd_x},
	{"p"    , "   Calculate the result of the given expression"                    ,     cmd_p},
	{"w"    , "   Set a new watchpoint to monitor the given expression"            ,     cmd_w},
	{"d"    , "   Delete the watchpoint with sequence number 'n'"                  ,     cmd_d},
	//{ "ext", "   Test your expr_function", cmd_t}
  
	/* TODO: Add more commands */
  
  }; 


// #define ARRLEN(arr) (int)(sizeof(arr) / sizeof(arr[0]))
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
	if(is_batch_mode)
	{
		cmd_c(NULL);
		return;
	}

	
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


  void init_sdb() {
	/* Compile the regular expressions. */
	init_regex();
  
	/* Initialize the watchpoint pool. */
	init_wp_pool();
  }