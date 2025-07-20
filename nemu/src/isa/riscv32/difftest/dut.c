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
#include <cpu/difftest.h>
#include "../local-include/reg.h"
#include <utils.h>

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  bool flag = true;
  if(ref_r->pc != cpu.pc) {
    Log("PC mismatch: DUT = " FMT_WORD ", REF = " FMT_WORD, cpu.pc, ref_r->pc);
    flag = false;
  }

  for(int i = 0; i < 32; i++)
  {
    if(cpu.gpr[i] != ref_r->gpr[i])
    {
      Log("GPR mismatch: DUT[%d] = " FMT_WORD ", REF[%d] = " FMT_WORD, i, cpu.gpr[i], i, ref_r->gpr[i]);
      flag = false;
    } 
  }
  return flag;
}

void isa_difftest_attach() {
}
