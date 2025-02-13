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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}"; 



#define MAX_DEPTH 10


int choose(int n)
{
  return rand() % n;
}

void gen_num() {
  sprintf(buf + strlen(buf), "%d", 1 + rand() % 100);  // 生成随机数字并拼接到buf
}

void gen_rand_op() {
  char ops[] = {'+', '-', '*', '/'};
  sprintf(buf + strlen(buf), " %c ", ops[choose(4)]);  // 生成随机运算符并拼接
}

void gen(char e) {
  sprintf(buf + strlen(buf), "%c", e);  // 将单字符添加到buf末尾
}

static void gen_rand_expr() {
    
    if (strlen(buf) >= 60000) {
        gen_num();  
        return;
    }

    switch(choose(3)) {
        case 0:
            gen_num();
            break;
        case 1:
            gen('(');
            gen_rand_expr();
            gen(')');
            break;
        default:
            gen_rand_expr();
            gen_rand_op();
            gen_rand_expr();
            break;
    }
}


bool check_zero(char *expr) //seemingly done
{
  for(int i = 0, length = strlen(expr); i < length; i++)
  {
    if(expr[i] == '/')
    {
      
      char temp_buf[65536] = {};
      int z = i + 1;
      int j = 0;
      if(expr[z] == '(')
      {
        int num = 1;
        sprintf(temp_buf + strlen(temp_buf), "%c", expr[z]);
        //assert(0);
        for(int k = z + 1; k <= length; k++)
        {
          sprintf(temp_buf + strlen(temp_buf), "%c", expr[k]);
          if(expr[k] == ')')
          {
            num--;
          }
          else if(expr[k] == '(')
          {
            num++;
          }
          if(num == 0)
          {
            break;
          }
        }
      }
      else
      {
        while(i < length && expr[i] != '+' && expr[i] != '-' && expr[i] != '*' && expr[i] != '/')
        { 
          temp_buf[j++] = expr[i++];
        }
        temp_buf[j] = '\0';
        
      }
      unsigned result = strtoul(temp_buf, NULL, 10);
      
      if(result == 0)
      {
        return false;
      }
      
    }
  }
  return true;
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    buf[0] = '\0';
    gen_rand_expr();
    
    /*while(!check_zero(buf))
    {
      buf[0] = '\0';
      gen_rand_expr(0);
    }*/
    

    sprintf(code_buf, code_format, buf); //code_format是buf插入code_buf的格式 相当于将buf格式化为c代码按照code_format的格式放入到code_buf

    FILE *fp = fopen("/home/leonard/ysyx-workbench/nemu/tools/gen-expr/temp.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc -Wno-overflow -Wall -Werror /home/leonard/ysyx-workbench/nemu/tools/gen-expr/temp.c -o /home/leonard/ysyx-workbench/nemu/tools/gen-expr/temp");
    if (ret != 0) continue;

    fp = popen("/home/leonard/ysyx-workbench/nemu/tools/gen-expr/temp", "r");
    assert(fp != NULL);

    int result;
    ret = fscanf(fp, "%u", &result);
    pclose(fp);

    printf("%x %s\n", result, buf);
  }
  return 0;
}









