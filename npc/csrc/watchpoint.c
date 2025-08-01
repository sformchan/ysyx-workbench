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

#include "state.h"
#include <stdbool.h>
#include "vinit.h"
#include <string.h>

uint32_t expr(char *e, bool *success);
#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  
  char *expr;
  uint32_t value;
  bool enable;

  /* TODO: Add more members if necessary */

} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
    wp_pool[i].enable = false;
    wp_pool[i].expr = NULL;
    wp_pool[i].value = 0;
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
WP *new_wp()
{
  if(free_ == NULL)
  {
    printf("no wp available\n");
    assert(0);
  }
  
  WP *wp = free_;
  free_ = free_->next;
  
  
  wp->next = head;
  head = wp;
  wp->enable = true;
  
  return wp;
}

void free_wp(WP *wp)
{
  if(wp == NULL)
  {
    printf("no wp working.\n");
    assert(0);
  }
  WP *prev = NULL;
  WP *cur = head;
  
  while(cur != NULL)
  {
    if(cur == wp)
    {
      if(prev == NULL)
      {
        head = cur->next;
      }
      else
      {
        prev->next = cur->next;
      }
      break; 
    }
    
    prev = cur;
    cur = cur->next;
    
  }
  wp->enable = false;
  wp->next = free_;
  free_ = wp;
}




void set_wp(char *expr_str)
{
  
  WP *wp = new_wp();          
  wp->expr = strdup(expr_str);
  bool success = false;
  wp->value = expr(wp->expr, &success);
  printf("watchpoint set successfully: No.%d --> %s\n", wp->NO, wp->expr);
  

}


void display_wp()
{
  if(head == NULL)
  {
    printf("No wp working.\n");
    return;
  }
  
  printf("Currently working watchpoints: \n");
  
  for(WP *wp = head; wp != NULL; wp = wp->next)
  {
    printf("Wp %d: Expression = %s,  Value = 0x%08x,  Enable = %s\n", wp->NO, wp->expr, wp->value, wp->enable ? "Yes" : "No");
    //printf("%s\n", wp->expr);
  }
}




void delete_wp(int num)
{
  if(head == NULL)
  {
    printf("No working wp to delete.\n");
  }
  WP *current = head;
  while(current != NULL)
  {
    if(current->NO == num)
    {
      free_wp(current);
      printf("Successfully deleted Wp %d\n", current->NO);
      return;
    }
    current = current->next;
  }
  printf("Wp %d not found.\n", num);
}


bool check_wp()
{
  WP *wp = head;
  while(wp != NULL)
  {
    bool success = false;
    uint32_t new_value = expr(wp->expr, &success);
    if(success && new_value != wp->value)
    {
      printf("Wp %d triggered at 0x%08x: " ANSI_FG_GREEN "0x%08x" ANSI_NONE " <-- 0x%08x (%s)\n",wp->NO, top->pc, new_value, wp->value, wp->expr);
      wp->value = new_value;
      return true;
    }
    wp = wp->next;
  } 
  return false;
}

