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
#include <ctype.h>
#include <memory/paddr.h>


bool check_parentheses(int p, int q);
uint32_t eval(int p, int q);
word_t isa_reg_str2val(const char *s, bool *success);


enum {
  TK_DEQ = 1,
  TK_NUM = 2,
  TK_NOTYPE = 256,
  TK_HEX = 16,
  TK_NEQ = 3,
  TK_REG = 4,
  TK_AND = 5,
  TK_OR = 6,
  TK_DEREF = 7,
  TK_NEG = 8
  

  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"\\=\\=", TK_DEQ},        // equal
  {"0x[0-9a-fA-F]+", TK_HEX}, //heximal
  {"[0-9]+", TK_NUM}, //decimal
  {"\\-", '-'}, //minus
  {"\\*", '*'}, //mutilply
  {"\\/", '/'}, //divide
  {"\\(", '('}, //left
  {"\\)", ')'}, //right
  {"\\!\\=", TK_NEQ},
  {"\\$\\w+", TK_REG},
  {"\\&\\&", TK_AND},
  {"\\|\\|", TK_OR}
  
  
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
          case 16:
            tokens[nr_token].type = 16;
            strncpy(tokens[nr_token].str, &e[position - substr_len], substr_len); // avoid overflow
            nr_token++;
            break; 
          case 2:
            tokens[nr_token].type = 2;
            strncpy(tokens[nr_token].str, &e[position - substr_len], substr_len); // avoid overflow
            nr_token++;
            break; 
          case 1:
            tokens[nr_token].type = 1;
            nr_token++;
            break;
          case 256:
            break;
          case '(':
            tokens[nr_token].type = '(';
            nr_token++;
            break;
          case ')':
            tokens[nr_token].type = ')';
            nr_token++;
            break;
          case 3:
            tokens[nr_token].type = 3;
            nr_token++;
            break;
          case 4:
            tokens[nr_token].type = 4; //reg
            strncpy(tokens[nr_token].str, &e[position - substr_len], substr_len);
            nr_token++;
            break;
          case 5:
            tokens[nr_token].type = 5;
            nr_token++;
            break;
          case 6:
            tokens[nr_token].type = 6;
            nr_token++;
            break;
          default: 
            printf(ANSI_FG_RED "ERROR" ANSI_NONE ": invalid input: %d %c\n", position, e[position]);
            break;
        }
        break;
      }
    }
  
    if  (i == NR_REGEX) {
        printf(ANSI_FG_RED "ERROR" ANSI_NONE ": no match at position %d\n%s\n%*.s^\n", position, e, position, "");
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
  //printf("result: 0x%x\n", eval(0, nr_token - 1));
  for(int i = 0; i < nr_token; i++)
  {
    if(tokens[i].type == '*' && (i == 0 || tokens[i - 1].type == '+' ||
                                           tokens[i - 1].type == '-' ||
                                           tokens[i - 1].type == '*' ||
                                           tokens[i - 1].type == '/' ||
                                           tokens[i - 1].type == '(' ))
    {
      
      tokens[i].type = 7;
    }
    if(tokens[i].type == '-' && (i == 0 || tokens[i - 1].type == '+' ||
                                           tokens[i - 1].type == '-' ||
                                           tokens[i - 1].type == '*' ||
                                           tokens[i - 1].type == '/' ||
                                           tokens[i - 1].type == '(' ))
    {
      
      tokens[i].type = 8;
    }
  }
  
  uint32_t result = eval(0, nr_token - 1);
  *success = true;
  return result;
  
}

bool check_parentheses(int p, int q)
{
  if(tokens[p].type == '(')
  {
    int num = 1;
    for(int i = p + 1; i <= q; i++)
    {
      if(tokens[i].type == ')')
      {
        num--;
        if(num == 0 && i == q)
        {
          return true;
        }
      }
      else if(tokens[i].type == ')')
      {
        num++;
      }
    }
  }
  return false;
}
/*bool check_parentheses(int p, int q)
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
}*/


uint32_t eval(int p, int q) {
  

  if (p == q) {
    /* Single token.
     * For now this token should be a number.
     * Return the value of the number.
     */ 
     bool success = false;
     if(tokens[p].type == 4)
     {
       word_t reg_value = isa_reg_str2val(tokens[p].str, &success);
       if(!success)
       {
         printf(ANSI_FG_RED "ERROR" ANSI_NONE ": NOT A LEGAL REGISTER\n");
         printf("please prompt again\n");
         return 0;
       }
       return reg_value;
     }
     else if(tokens[p].type == 16)
     {
       word_t hex_value = (word_t)strtol(tokens[p].str, NULL, 16); 
       return hex_value;
     }
     else if(tokens[p].type == 2)
     {
       return atoi(tokens[p].str);
     }
     else
     {
       printf(ANSI_FG_RED "ERROR" ANSI_NONE ": NOT A LEGAL EXPRESSION\n");
       printf("please prompt again\n");
       return 0;
     }
     
  }
  else if(tokens[p].type == 7)
  {
    
    word_t addr = eval(p + 1, q);
    word_t data = paddr_read(addr, 4);
    return data;
  }
  else if(tokens[p].type == 8)
  {
    word_t result = eval(p + 1, q);
    return -1 * result;
  }
  else if (check_parentheses(p, q) == true) {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
    assert(0);
    return eval(p + 1, q - 1);
  }
  else {
    //find major
    
    int op = -1;
    int sign = 0;
    
    for(int i = p; i < q; i++)
    {
      
      if(tokens[i].type == '(')
      {
        int num = 1;
        i++;
        while(num != 0)
        {
          if(tokens[i].type == '(')
          {
            num++;
          }
          if(tokens[i].type == ')')
          {
            num--;
          }
          i++;
        }
        //printf("%d\n", i); 
      }
      if((sign >= 1 || sign == 0) && (tokens[i].type == '+' || tokens[i].type == '-'))
      {
        //assert(0);
        sign = 1;
        op = i;
      }
      if((sign >= 2 || sign == 0) && (tokens[i].type == '*' || tokens[i].type == '/'))
      {
        sign = 2;
        op = i;
      }
      if((sign >= 5 || sign == 0) && (tokens[i].type == 1 || tokens[i].type == 3))
      {
        //assert(0);
        sign = 5;
        op = i;
      }
      if((sign >= 3 || sign == 0) && tokens[i].type == 5)
      {
        sign = 3;
        op = i;
      }
      if((sign >= 4 || sign == 0) && tokens[i].type == 6)
      {
        sign = 4;
        op = i;
      }
      
    }
    //printf("%d\n", op);
    //printf("flag is %s\n", sign ? "true" : "false");
    int op_type = tokens[op].type;
    printf("%d\n", op_type);
    //op = the position of 主运算符 in the token expression;
    if(p > op - 1 || op + 1 > q)
    {
      printf("%d %d %d %d\n", p, op-1, op+1, q);
      assert(0);
      printf(ANSI_FG_RED "ERROR" ANSI_NONE ": NOT A LEGAL EXPRESSION\n");
      printf("please prompt again\n");
      return 0;
    }
    
    uint32_t val1 = eval(p, op - 1);
    uint32_t val2 = eval(op + 1, q);
    
    

    switch (op_type) {
      case '+': return val1 + val2;
      case '-': return val1 - val2;
      case '*': return val1 * val2;
      case '/': 
        if(val2 == 0)
        {
          printf(ANSI_FG_RED "ERROR" ANSI_NONE ": cannot divised by ZERO\n");
          return 0;
        }
        else
          return val1 / val2;
      case 1: return val1 == val2;
      case 3: return val1 != val2;
      case 5: return val1 && val2;
      case 6: return val1 || val2;
      default:
        {
          printf(ANSI_FG_RED "ERROR" ANSI_NONE ": NOT A LEGAL EXPRESSION\n");
          printf("please prompt again\n");
          return 0;
        }
    }
  }
} 


