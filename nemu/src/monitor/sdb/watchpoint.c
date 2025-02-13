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

#include "sdb.h"
word_t expr(char *e, bool *success);
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
  //printf("%p\n", wp->expr);
  char real_expr[100];
  real_expr[0] = '*';
  strcpy(real_expr + 1, wp->expr);
  bool success = false;
  wp->value = expr(real_expr, &success);
  if(!success)
  {
    printf("invalid input: %s\n", wp->expr);
  }
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
    printf("Wp %d: Expression = %s,  Value = 0x%x,  Enable = %s\n", wp->NO, wp->expr, wp->value, wp->enable ? "Yes" : "No");
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
    char real_expr[100];
    real_expr[0] = '*';
    strcpy(real_expr + 1, wp->expr);
    uint32_t new_value = expr(real_expr, &success);
    if(success && new_value != wp->value)
    {
      printf("wp triggered at %s: 0x%x <-- 0x%x\n", wp->expr, new_value, wp->value);
      return true;
    }
    wp = wp->next;
  } 
  return false;
}

