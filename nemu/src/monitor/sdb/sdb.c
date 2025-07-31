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

void expr_cp(FILE *file)
{
  char line[65536];
  int num[100];
  int i = 0;
  int count = 1;
  while(fgets(line, sizeof(line), file))
  {
    line[strcspn(line, "\n")] = 0;
    
    char *result0 = strtok(line, " ");
    char *expression = strtok(NULL, "");
    bool success = false;
    unsigned result1 = expr(expression, &success);
    if(result0 && expression)
    {
      printf(ANSI_FG_WHITE "%d" ANSI_NONE ANSI_FG_GREEN " %s %x " ANSI_NONE "  %s\n", count, result0, result1, expression);
      unsigned result2 = strtol(result0, NULL, 16);
      if(result1 != result2)
      {
        num[i++] = count;
      }
    }
    count++;
  }
  printf("\n"); 
  printf("pass:%d all:%d\n", count - i - 1, count - 1);
  if(i > 0)
  {
    printf("where it went wrong: ");
    for(int j = 0; j < i; j++)
    {
      printf("%d ", num[j]);
    }
    printf("\n");
  }
  else
  {
    printf(ANSI_FG_YELLOW"EVERY RESULT IS CORRECT!\n"ANSI_NONE);
  }
}

static std::string readline(int fd)
{
  struct termios tios;
  // try to make sure the terminal is noncanonical and nonecho
  if (tcgetattr(fd, &tios) == 0)
  {
    tios.c_lflag &= (~ICANON);
    tios.c_lflag &= (~ECHO);
    tcsetattr(fd, TCSANOW, &tios);
  }
  bool noncanonical = tcgetattr(fd, &tios) == 0 && (tios.c_lflag & ICANON) == 0;

  std::string s_head = std::string("(spike) ");
  std::string s = s_head;
  keybuffer_t key_buffer = 0;
  // index for up/down arrow
  size_t history_index = 0;
  // position for left/right arrow
  size_t cursor_pos = s.size();
  const size_t initial_s_len = cursor_pos;
  std::cerr << s << std::flush;
  for (char ch; read(fd, &ch, 1) == 1; )
  {
    uint32_t keycode = key_buffer << BITS_PER_CHAR | ch;
    switch (keycode)
    {
      // the partial keycode, add to the key_buffer
      case KEYCODE_HEADER0:
      case KEYCODE_HEADER1:
      case KEYCODE_HOME1_0:
      case KEYCODE_END1_0:
      case KEYCODE_BACKSPACE1_0:
        key_buffer = keycode;
        break;
      // for backspace key
      case KEYCODE_BACKSPACE0:
      case KEYCODE_BACKSPACE1_1:
      case KEYCODE_BACKSPACE2:
        if (cursor_pos <= initial_s_len)
          continue;
        clear_str(noncanonical, fd, s);
        cursor_pos--;
        s.erase(cursor_pos, 1);
        if (noncanonical && write(fd, s.c_str(), s.size() + 1) != 1)
          ; // shut up gcc
        // move cursor by left arrow key
        for (unsigned i = 0; i < s.size() - cursor_pos; i++) {
          send_key(noncanonical, fd, KEYCODE_LEFT, 3);
        }
        key_buffer = 0;
        break;
      case KEYCODE_HOME0:
      case KEYCODE_HOME1_1:
        // move cursor by left arrow key
        for (unsigned i = 0; i < cursor_pos - initial_s_len; i++) {
          send_key(noncanonical, fd, KEYCODE_LEFT, 3);
        }
        cursor_pos = initial_s_len;
        key_buffer = 0;
        break;
      case KEYCODE_END0:
      case KEYCODE_END1_1:
        // move cursor by right arrow key
        for (unsigned i = 0; i < s.size() - cursor_pos; i++) {
          send_key(noncanonical, fd, KEYCODE_RIGHT, 3);
        }
        cursor_pos = s.size();
        key_buffer = 0;
        break;
      case KEYCODE_UP:
        // up arrow
        if (history_commands.size() > 0) {
          clear_str(noncanonical, fd, s);
          history_index = std::min(history_commands.size(), history_index + 1);
          s = history_commands[history_commands.size() - history_index];
          if (noncanonical && write(fd, s.c_str(), s.size() + 1))
            ; // shut up gcc
          cursor_pos = s.size();
        }
        key_buffer = 0;
        break;
      case KEYCODE_DOWN:
        // down arrow
        if (history_commands.size() > 0) {
          clear_str(noncanonical, fd, s);
          history_index = std::max(0, (int)history_index - 1);
          if (history_index == 0) {
            s = s_head;
          } else {
            s = history_commands[history_commands.size() - history_index];
          }
          if (noncanonical && write(fd, s.c_str(), s.size() + 1))
            ; // shut up gcc
          cursor_pos = s.size();
        }
        key_buffer = 0;
        break;
      case KEYCODE_LEFT:
        if (s.size() > initial_s_len) {
          cursor_pos = cursor_pos - 1;
          if ((int)cursor_pos < (int)initial_s_len) {
            cursor_pos = initial_s_len;
          } else {
            send_key(noncanonical, fd, KEYCODE_LEFT, 3);
          }
        }
        key_buffer = 0;
        break;
      case KEYCODE_RIGHT:
        if (s.size() > initial_s_len) {
          cursor_pos = cursor_pos + 1;
          if (cursor_pos > s.size()) {
            cursor_pos = s.size();
          } else {
            send_key(noncanonical, fd, KEYCODE_RIGHT, 3);
          }
        }
        key_buffer = 0;
        break;
      case KEYCODE_ENTER:
        if (noncanonical && write(fd, &ch, 1) != 1)
          ; // shut up gcc
        if (s.size() > initial_s_len && (history_commands.size() == 0 || s != history_commands[history_commands.size() - 1])) {
          history_commands.push_back(s);
        }
        return s.substr(initial_s_len);
      default:
      DEFAULT_KEY:
        // unknown buffered key, do nothing
        if (key_buffer != 0) {
          key_buffer = 0;
          break;
        }
        clear_str(noncanonical, fd, s);
        s.insert(cursor_pos, 1, ch);
        cursor_pos++;
        if (noncanonical && write(fd, s.c_str(), s.size() + 1) != 1)
          ; // shut up gcc
        // send left arrow key to move cursor
        for (unsigned i = 0; i < s.size() - cursor_pos; i++) {
          send_key(noncanonical, fd, KEYCODE_LEFT, 3);
        }
        break;
    }
  }
  return s.substr(initial_s_len);
}

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
    printf(ANSI_FG_RED "ERROR" ANSI_NONE ": try entering 'w' or 'r'.\n");
  }
  else
  {
    sscanf(args, "%c", &letter);
    if(letter == 'r')
    {
      printf(ANSI_FG_GREEN "PC : 0x%08x\n" ANSI_NONE, cpu.pc);
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
    printf(ANSI_FG_RED "ERROR" ANSI_NONE ": nothing output cause of INVALID INPUT.\n");
  }
  else
  {
    sscanf(args, "%d %s", &length, start);
    uint32_t result = expr(start, &success);
    //printf("%x\n", result);
    if( length > 0 && (result >= 0x80000000 && result <= 0x87ffffff))
    {
      for(int i = 0; i < length; i++)
      {
        printf("%d 0x%08x 0x%08x\n", i, result + (i * 4), vaddr_read(result + (i * 4), 4));
      }
    }
    else
    {
      printf(ANSI_FG_RED "ERROR" ANSI_NONE ": nothing output cause of INVALID INPUT.\n");
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


static int cmd_t(char *args)
{
  if(args == NULL)
  {
    FILE *file = fopen("/home/leonard/ysyx-workbench/nemu/tools/gen-expr/input", "r");
    if(!file)
    {
      perror("failed to open file");
    }
    expr_cp(file);
    fclose(file);
    return 0;
  }
  else
  {
    printf(ANSI_FG_RED "ERROR" ANSI_NONE ": nothing output cause of INVALID INPUT.\n");
    return 0;
  }
}


static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  {"help" , ANSI_FG_CYAN "Display information about all supported commands" ANSI_NONE , cmd_help },
  { "c", "   Continue the execution of the program", cmd_c },
  { "q", ANSI_FG_CYAN "   Exit NEMU" ANSI_NONE , cmd_q },
  { "si", "  Let the program step, you can enter a number after 'si'", cmd_si },
  { "info", ANSI_FG_CYAN "Print info of reg or wp" ANSI_NONE, cmd_info },
  { "x", "   Visit the target memory and print it, you are expected to enter an expression", cmd_x},
  { "p", ANSI_FG_CYAN "   Calculate the result of the given expression" ANSI_NONE, cmd_p},
  { "w", "   Set a new watchpoint to monitor the given expression", cmd_w},
  { "d", ANSI_FG_CYAN "   Delete the watchpoint with sequence number 'n'" ANSI_NONE, cmd_d},
  { "ext", "   Test your expr_function", cmd_t}

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
