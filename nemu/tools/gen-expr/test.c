#include <stdio.h>
#include <stdbool.h>



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
        if(num == 0 && i != q)
        {
          return false;
        }
        if(num == 0 && i == q)
        {
          return true;
        }
        
      }
      else if(tokens[i].type == '(')
      {
        num++;
      }
    }
  }
  return false;
}



uint32_t eval(int p, int q) {
  

  if (p == q) {
     return atoi(tokens[p].str);  
  }
  
  else if (check_parentheses(p, q) == true) {
    return eval(p + 1, q - 1);
  }
  else {
    //find major
    
    int op = -1;
    int sign = 0;
    for(int i = p; i <= q; i++)
    {
      
      if(tokens[i].type == '(')
      {
        int num = 1;
        //assert(0);
        for(int j = i + 1; j <= q; j++)
        {
          i++;
          if(tokens[j].type == ')')
          {
            num--;
          }
          else if(tokens[j].type == '(')
          {
            num++;
          }
          if(num == 0)
          {
            break;
          }
          //printf("hello\n");
        }
         
      }
      if((sign >= 1 || sign == 0) && (tokens[i].type == '+' || tokens[i].type == '-'))
      {
        //assert(0);
        sign = 1;
        op = i;
      }
      if((sign >= 2 || sign == 0) && (tokens[i].type == '*' || tokens[i].type == '/'))
      {
        //printf("hello\n");
        sign = 2;
        op = i;
      }
      
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
      default:
        {
          printf(ANSI_FG_RED "ERROR" ANSI_NONE ": NOT A LEGAL EXPRESSION\n");
          printf("please prompt again\n");
          return 0;
        }
    }
  }
} 

