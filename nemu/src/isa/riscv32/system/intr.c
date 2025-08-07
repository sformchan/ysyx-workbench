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
#include <utils.h>

const char *exception_name(int mcause) {
	switch (mcause) {
	  case 0: return "Instruction address misaligned";
	  case 2: return "Illegal instruction";
	  case 11: return ANSI_FG_YELLOW "Environment call from M-mode" ANSI_NONE;
	  // 根据需要补充更多异常
	  default: return "Unknown exception";
	}
  }

void etrace(int mcause, vaddr_t mepc) {
	printf(ANSI_FG_WHITE "[etrace]" ANSI_NONE " mcause=0x%x (%s), mepc=0x%08x\n", mcause, exception_name(mcause), mepc);
	printf("\n");
	printf("gpr: ");
	for (int i = 0; i < 32; i++) {
	  printf("x%d=0x%08x\n", i, cpu.gpr[i]);
	}
	printf("\n");
  
	printf("mstatus=0x%x mepc=0x%08x\n", cpu.mstatus, cpu.mepc);
  }

word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */
  cpu.mcause = NO;
  cpu.mepc = epc;
  if (NO == 11) {
    cpu.mepc += 4;  // 跳过ecall，防止死循环
  }
//   word_t vec = cpu.mtvec;  
//   printf("[EXCEPTION] raise NO=0x%x from epc=0x%08x, jump to mtvec=0x%08x\n", NO, epc, vec);
	etrace(NO, epc);
  return cpu.mtvec;
}

word_t isa_query_intr() {
  return INTR_EMPTY;
}
