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

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>



bool check_parentheses(int p, int q);
uint32_t eval(int p, int q);



enum {
  TK_EQ = 1,
  TK_NUM = 2,
  TK_NOTYPE = 256,
  

  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {"\\s+", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"==", TK_EQ},        // equal
  {"[0-9]", TK_NUM},
  {"\\-", '-'},
  {"\\*", '*'},
  {"\\/", '/'},
  {"\\(", '('},
  {"\\)", ')'}
};

#define NR_REGEX ARRLEN(rules)  //calculate the length of array

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new tok en is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case '+':
            tokens[nr_token].type = '+';
            nr_token++;
            break;
          case '-':
            tokens[nr_token].type = '-';
            nr_token++;
            break;
          case '*':
            tokens[nr_token].type = '*';
            nr_token++;
            break;
          case '/':
            tokens[nr_token].type = '/';
            nr_token++;
            break;  
          case 2:
            tokens[nr_token].type = 2;
            strncpy(tokens[nr_token].str, &e[position - substr_len], substr_len);
            nr_token++;
            break;
          case 1:
            tokens[nr_token].type = 1;
            nr_token++;
            break;
          case 256:
            tokens[nr_token].type = 256;
            nr_token++;
            break;
          default: 
            printf("invalid input: %d\n ", i);
            break;
        }
        break;
      }
    }
  
    if  (i == NR_REGEX) {
        printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
        return false;
    }
  }
  return true;
}


word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  printf("result: 0x%x\n", eval(0, strlen(e) - 1));

  return 0;
}


bool check_parentheses(int p, int q)
{
  if(tokens[p].type != '('  || tokens[q].type != ')')
    return false;
      int l = p , r = q;
  while(l < r)
  {
    if(tokens[l].type == '('){
      if(tokens[r].type == ')')
      {       
        l ++ , r --;
          continue;
      }
      else       
        r --;
    }
    else if(tokens[l].type == ')')
      return false;
    else l ++;
  }
  return true;
}


#define max(a, b) ((a) > (b) ? (a) : (b))


uint32_t eval(int p, int q) {
  bool sign = false;
  if (p > q) {
    /* Bad expression */
    printf("%d", sign);
    printf("eval(%d, %d)\n", p, q);
    assert(0);
    return -1;
  }
  else if (p == q) {
    /* Single token.
     * For now this token should be a number.
     * Return the value of the number.
     */ 
     return atoi(tokens[p].str);
     
  }
  else if (check_parentheses(p, q) == true) {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
    return eval(p + 1, q - 1);
  }
  else {
    int op = -1;
    
    
    for(int i = p; i <= q; i++)
    {
      if(tokens[i].type == '(')
      {
        while(tokens[i].type != ')')
        {
          i++;
        }
      }
      if(!sign && (tokens[i].type == '*' || tokens[i].type == '/'))
      {
        op = max(op, i);
      }
      if(!sign && (tokens[i].type == '+' || tokens[i].type == '-'))
      {
        sign = true;
        op = max(op, i);
      }
      
    }
    
    int op_type = tokens[op].type;
    
    //op = the position of 主运算符 in the token expression;
    uint32_t val1 = eval(p, op - 1);
    uint32_t val2 = eval(op + 1, q);

    switch (op_type) {
      case '+': return val1 + val2;
      case '-': return val1 - val2;
      case '*': return val1 * val2;
      case '/': return val1 / val2;
      default: assert(0);
    }
  }
} 



