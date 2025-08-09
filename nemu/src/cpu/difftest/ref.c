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
#include <difftest-def.h>
#include <memory/paddr.h>

void diff_set_regs(void *ctx) {
	CPU_state *c = (CPU_state *)ctx;
	for (int i = 0; i < 32; i++) {
	  cpu.gpr[i] = c->gpr[i];
	}
	cpu.pc = c->pc;
	cpu.mepc = c->mepc;
	cpu.mtvec = c->mtvec;
	cpu.mcause = c->mcause;
	cpu.mstatus = c->mstatus;
  }
  
void diff_get_regs(void *ctx) {
	CPU_state *c = (CPU_state *)ctx;
	for (int i = 0; i < 32; i++) {
	  c->gpr[i] = cpu.gpr[i];
	}
	c->pc = cpu.pc;
	c->mepc = cpu.mepc;
	c->mtvec = cpu.mtvec;
	c->mcause = cpu.mcause;
	c->mstatus = cpu.mstatus;
}
  

__EXPORT void difftest_memcpy(paddr_t addr, void *buf, size_t n, bool direction) {
	if(direction == DIFFTEST_TO_REF) memcpy(guest_to_host(addr), buf, n);
	else assert(0);
}

__EXPORT void difftest_regcpy(void *dut, bool direction) {
	if (direction == DIFFTEST_TO_REF) {
		// 把 DUT 的寄存器值写入 REF
		diff_set_regs(dut);
	} 
	else {
		// 把 REF 的寄存器值读出，写入 DUT
		diff_get_regs(dut);
	  }
  	//assert(0);
}

__EXPORT void difftest_exec(uint64_t n) {
	for(int i = 0; i < n; i++) cpu_exec(1);
  //assert(0);
}

__EXPORT void difftest_raise_intr(word_t NO) {
    // 触发异常编号NO
    cpu.mcause = NO;
    // 保存当前pc到mepc
    cpu.mepc = cpu.pc;
    // 设置mstatus，屏蔽中断等（参考你之前写的）
    cpu.mstatus &= ~(1 << 7);               // MPIE = MIE
    cpu.mstatus |= ((cpu.mstatus & (1 << 3)) << 4);  // MPP = previous MIE
    cpu.mstatus &= ~(1 << 3);               // MIE = 0
    cpu.mstatus |= (1 << 11) | (1 << 12);   // 设置 MPP 为机器模式 (假设机器模式为 3)

    // 跳转到中断向量
    cpu.pc = cpu.mtvec;
}

__EXPORT void difftest_init(int port) {
  void init_mem();
  init_mem();		
  /* Perform ISA dependent initialization. */
  init_isa();
}
