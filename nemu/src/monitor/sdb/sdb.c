/***************************************************************************************
* Copyright (c) 2014-2024 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"
#include <memory/paddr.h>
#include <memory/vaddr.h>

static int is_batch_mode = false;
void set_wp(char *expr_str);
void init_regex();
void init_wp_pool();
void display_wp();
void delete_wp(int num);
word_t expr(char *e, bool *success);


/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
  nemu_state.state = NEMU_QUIT;
  return -1;
}

static int cmd_help(char *args);

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
  cpu_exec(step);
  return 0;
}


static int cmd_info(char *args)
{
  char letter;
  if(args == NULL)
  {
    printf("nothing output cause of INVALID INPUT.");
  }
  else
  {
    sscanf(args, "%c", &letter);
    if(letter == 'r')
    {
      isa_reg_display();
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
  if(args == NULL)
  {
    printf("nothing output cause of INVALID INPUT.");
  }
  else
  {
    sscanf(args, "%d %s", &length, start);
    uint32_t result = expr(start, &success);
    if( length > 0 && result >= 0x80000000)
    {
      for(int i = 0; i < length; i++)
      {
        printf("%d 0x%x 0x%x\n", i, result + (i * 4), vaddr_read(result + (i * 4), 4));
      }
    }
    else
    {
      printf("nothing output cause of INVALID INPUT.\n");
    }  
  }
  return 0;
}



static int cmd_p(char *args)
{
  if(args == NULL)
  {
    printf("nothing output cause of INVALID INPUT.");
    return 0;
  }
  bool sign = false;
  printf("result: 0x%x\n", expr(args, &sign));
  return 0;
}



static int cmd_w(char *args)
{
  if(args == NULL)
  {
    printf("nothing output cause of INVALID INPUT.");
    return 0;
  }
  set_wp(args);
  return 0;
}



static int cmd_d(char *args)
{
  
  if(args == NULL)
  {
    printf("nothing output cause of INVALID INPUT.");
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
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Let the program step, you can enter a number after 'si'", cmd_si },
  { "info", "Print info of reg or wp", cmd_info },
  { "x", "Visit the target memory and print it, you are expected to enter a", cmd_x},
  { "p", "Calculate the result of the given expression", cmd_p},
  { "w", "Set a new watchpoint to monitor the given expression", cmd_w},
  { "d", "Delete the watchpoint with sequence number 'n'", cmd_d}

  /* TODO: Add more commands */

}; 

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
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

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

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
