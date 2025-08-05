#ifndef __DIFFTEST_DEF_H__
#define __DIFFTEST_DEF_H__

#include <stdint.h>

typedef uint32_t word_t;
typedef uint32_t vaddr_t;

typedef struct {
  word_t gpr[32];   // 32 个通用寄存器
  vaddr_t pc;       // 程序计数器
} CPU_state;

#define DIFFTEST_TO_DUT false
#define DIFFTEST_TO_REF true

#endif
