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

#include "local-include/reg.h"
#include <cpu/cpu.h>
#include <cpu/ifetch.h>
#include <cpu/decode.h>
#include <limits.h>

#define R(i) gpr(i)
#define Mr vaddr_read
#define Mw vaddr_write


word_t csr_read(uint32_t csr_num) {
	//printf("csr read successfully\n");
	switch (csr_num) {
	  case 0x305: return cpu.csr.mtvec;     // mtvec
	  case 0x341: return cpu.csr.mepc;      // mepc
	  case 0x342: return cpu.csr.mcause;    // mcause
	  case 0x300: return cpu.csr.mstatus;   // mstatus
	  default:
		panic("Unhandled CSR read: 0x%x", csr_num);
	}
  }
  
void csr_write(uint32_t csr_num, word_t val) {
	switch (csr_num) {
	  case 0x305: cpu.csr.mtvec = val; break;   // mtvec
	  case 0x341: cpu.csr.mepc = val;  break;    // mepc
	  case 0x342: cpu.csr.mcause = val; break;  // mcause
	  case 0x300: cpu.csr.mstatus = val; break; // mstatus
	  default:
		panic("Unhandled CSR write: 0x%x", csr_num);
	}
	//printf("csr write successfully\n");
}

void exec_mret() {
	cpu.csr.mstatus &= ~(1<<3); 
	cpu.csr.mstatus |= ((cpu.csr.mstatus&(1<<7))>>4); 
	cpu.csr.mstatus |= (1<<7); 
	cpu.csr.mstatus &= ~((1<<11)+(1<<12)); 
}


void ftrace_exec(uint32_t pc, uint32_t target, uint32_t rd, uint32_t rs1, int32_t imm);

enum {
  TYPE_I, TYPE_U, TYPE_S,
  TYPE_N, TYPE_J, TYPE_R,
  TYPE_B // none
};

#define src1R() do { *src1 = R(rs1); } while (0)
#define src2R() do { *src2 = R(rs2); } while (0)
#define immI() do { *imm = SEXT(BITS(i, 31, 20), 12); } while(0)
#define immU() do { *imm = SEXT(BITS(i, 31, 12), 20) << 12; } while(0)
#define immS() do { *imm = (SEXT(BITS(i, 31, 25), 7) << 5) | BITS(i, 11, 7); } while(0)
#define immB() do { *imm = (BITS(i, 31, 31) << 12) | (BITS(i, 7, 7) << 11) | (BITS(i, 30, 25) << 5) | (BITS(i, 11, 8) << 1); } while (0)
#define immJ() do { *imm = (BITS(i, 31, 31) << 20) | (BITS(i, 19, 12) << 12) | (BITS(i, 20, 20) << 11) |(BITS(i, 30, 21) << 1); } while(0);

static void decode_operand(Decode *s, int *rd, word_t *src1, word_t *src2, word_t *imm, int type) {
  uint32_t i = s->isa.inst;
  int rs1 = BITS(i, 19, 15);
  int rs2 = BITS(i, 24, 20);
  *rd     = BITS(i, 11, 7);
  switch (type) {
    case TYPE_I: src1R();          immI(); break;
    case TYPE_U:                   immU(); break;
    case TYPE_S: src1R(); src2R(); immS(); break;
    case TYPE_J:                   immJ(); break;
    case TYPE_R: src1R(); src2R();         break;
    case TYPE_B: src1R(); src2R(); immB(); break;
    case TYPE_N:                           break;
    default: panic("unsupported type = %d", type);
  }
}

int mret_flag = 0;

static int decode_exec(Decode *s) {
  s->dnpc = s->snpc;

#define INSTPAT_INST(s) ((s)->isa.inst)
#define INSTPAT_MATCH(s, name, type, ... /* execute body */ ) { \
  int rd = 0; \
  word_t src1 = 0, src2 = 0, imm = 0; \
  decode_operand(s, &rd, &src1, &src2, &imm, concat(TYPE_, type)); \
  __VA_ARGS__ ; \
}

  INSTPAT_START();
  INSTPAT("??????? ????? ????? 111 ????? 00100 11", andi   , I, R(rd) = src1 & imm);
  INSTPAT("??????? ????? ????? 100 ????? 00100 11", xori   , I, R(rd) = src1 ^ imm);
  INSTPAT("0000000 ????? ????? 101 ????? 00100 11", srli   , I, R(rd) = src1 >> BITS(imm, 4, 0));
  INSTPAT("0000000 ????? ????? 001 ????? 00100 11", slli   , I, R(rd) = src1 << BITS(imm, 4, 0));
  INSTPAT("0100000 ????? ????? 101 ????? 00100 11", srai   , I, R(rd) = (signed)src1 >> BITS(imm, 4, 0));  //signed
  INSTPAT("??????? ????? ????? 100 ????? 11000 11", blt    , B, s->dnpc = ((signed)src1 < (signed)src2) ? (SEXT(imm, 13) + s->pc) : s->dnpc);
  INSTPAT("??????? ????? ????? 110 ????? 11000 11", bltu   , B, s->dnpc = (src1 < src2) ? (SEXT(imm, 13) + s->pc) : s->dnpc);
  INSTPAT("??????? ????? ????? 111 ????? 11000 11", bgeu   , B, s->dnpc = (src1 >= src2) ? (SEXT(imm, 13) + s->pc) : s->dnpc); //unsigned
  INSTPAT("??????? ????? ????? 101 ????? 11000 11", bge    , B, s->dnpc = ((signed)src1 >= (signed)src2) ? (SEXT(imm, 13) + s->pc) : s->dnpc);
  INSTPAT("??????? ????? ????? 001 ????? 11000 11", bne    , B, s->dnpc = (src1 != src2) ? (SEXT(imm, 13) + s->pc) : s->dnpc);
  INSTPAT("??????? ????? ????? 000 ????? 11000 11", beq    , B, s->dnpc = (src1 == src2) ? (SEXT(imm, 13) + s->pc) : s->dnpc);
  INSTPAT("??????? ????? ????? 010 ????? 00100 11", slti   , I, R(rd) = ((signed)src1 < (signed)imm) ? 1 : 0);
  INSTPAT("??????? ????? ????? 011 ????? 00100 11", sltiu  , I, R(rd) = (src1 < imm) ? 1 : 0); //unsigned
  INSTPAT("0100000 ????? ????? 000 ????? 01100 11", sub    , R, R(rd) = src1 - src2);
  INSTPAT("0000000 ????? ????? 000 ????? 01100 11", add    , R, R(rd) = src1 + src2);
  INSTPAT("0000000 ????? ????? 111 ????? 01100 11", and    , R, R(rd) = src1 & src2);
  INSTPAT("0000000 ????? ????? 110 ????? 01100 11", or     , R, R(rd) = src1 | src2);
  INSTPAT("0000000 ????? ????? 100 ????? 01100 11", xor    , R, R(rd) = src1 ^ src2);
  INSTPAT("0000001 ????? ????? 100 ????? 01100 11", div    , R, if(src2 == 0) R(rd) = -1; else if((signed)src1 == INT_MIN && (signed)src2 == -1) R(rd) = INT_MIN; else R(rd) = (signed)src1 / (signed)src2);  //M
  INSTPAT("0000001 ????? ????? 101 ????? 01100 11", divu   , R, if(src2 == 0) R(rd) = -1; else R(rd) = src1 / src2);  //M
  INSTPAT("0000001 ????? ????? 110 ????? 01100 11", rem    , R, if(src2 == 0) R(rd) = (signed)src1; else if((signed)src1 == INT_MIN && (signed)src2 == -1) R(rd) = 0; else R(rd) = (signed)src1 % (signed)src2);  //M
  INSTPAT("0000001 ????? ????? 111 ????? 01100 11", remu   , R, R(rd) = src1 % src2);  //M
  INSTPAT("0000001 ????? ????? 000 ????? 01100 11", mul    , R, R(rd) = BITS(src1 * src2, 31, 0));  //M
  INSTPAT("0000001 ????? ????? 001 ????? 01100 11", mulh   , R, R(rd) = (SEXT(src1, 32) * SEXT(src2, 32)) >> 32); //M
  //mulhsu  mulhu
  INSTPAT("0000001 ????? ????? 011 ????? 01100 11", mulhu  , R, R(rd) = ((uint64_t)src1 * (uint64_t)src2) >> 32);
  INSTPAT("0000000 ????? ????? 010 ????? 01100 11", slt    , R, R(rd) = ((signed)src1 < (signed)src2) ? 1 : 0);
  INSTPAT("0000000 ????? ????? 011 ????? 01100 11", sltu   , R, R(rd) = (src1 < src2) ? 1 : 0);
  INSTPAT("0100000 ????? ????? 101 ????? 01100 11", sra    , R, R(rd) = (signed)src1 >> BITS(src2, 4, 0));
  INSTPAT("0000000 ????? ????? 101 ????? 01100 11", srl    , R, R(rd) = src1 >> BITS(src2, 4, 0));
  INSTPAT("0000000 ????? ????? 001 ????? 01100 11", sll    , R, R(rd) = src1 << BITS(src2, 4, 0));
  INSTPAT("??????? ????? ????? 010 ????? 00000 11", lw     , I, R(rd) = Mr(src1 + imm, 4));
  INSTPAT("??????? ????? ????? 000 ????? 11001 11", jalr   , I, s->dnpc = (imm + src1) & ~1;R(rd) = s->snpc;    //set the least significant bit to 0
  #ifdef CONFIG_FTRACE 
  {ftrace_exec(s->pc, s->dnpc, rd, BITS(s->isa.inst, 19, 15), imm);} 
  #endif  
  );
  INSTPAT("??????? ????? ????? 010 ????? 01000 11", sw     , S, Mw(src1 + imm, 4, src2));
  INSTPAT("??????? ????? ????? 001 ????? 01000 11", sh     , S, Mw(src1 + imm, 2, src2));
  INSTPAT("??????? ????? ????? 000 ????? 01000 11", sb     , S, Mw(src1 + imm, 1, src2));
  INSTPAT("??????? ????? ????? ??? ????? 11011 11", jal    , J, s->dnpc = SEXT(imm, 21) + s->pc; R(rd) = s->snpc;
  #ifdef CONFIG_FTRACE
  {ftrace_exec(s->pc, s->dnpc, rd, 65, imm);}
  #endif
  );
  INSTPAT("??????? ????? ????? 000 ????? 00100 11", addi   , I, R(rd) = imm + src1); 
  INSTPAT("??????? ????? ????? ??? ????? 00101 11", auipc  , U, R(rd) = s->pc + imm);
  INSTPAT("??????? ????? ????? ??? ????? 01101 11", lui    , U, R(rd) = imm);
  INSTPAT("??????? ????? ????? 001 ????? 00000 11", lh     , I, R(rd) = SEXT(Mr(src1 + imm, 2), 16));
  INSTPAT("??????? ????? ????? 101 ????? 00000 11", lhu    , I, R(rd) = Mr(src1 + imm, 2));
  INSTPAT("??????? ????? ????? 100 ????? 00000 11", lbu    , I, R(rd) = Mr(src1 + imm, 1));
  INSTPAT("??????? ????? ????? 000 ????? 00000 11", lb     , I, R(rd) = SEXT(Mr(src1 + imm, 1), 8));
  INSTPAT("??????? ????? ????? 110 ????? 00100 11", ori    , I, R(rd) = src1 | imm); 
  INSTPAT("??????? ????? ????? 001 ????? 11100 11", csrrw  , I, R(rd) = csr_read(BITS(s->isa.inst, 31, 20)), csr_write(BITS(s->isa.inst, 31, 20), src1)); 
  INSTPAT("??????? ????? ????? 010 ????? 11100 11", csrrs  , I, word_t old = csr_read(BITS(s->isa.inst, 31, 20)); R(rd) = old; csr_write(BITS(s->isa.inst, 31, 20), src1 | old)); 

  INSTPAT("0000000 00001 00000 000 00000 11100 11", ebreak , N, NEMUTRAP(s->pc, R(10)); /*printf("R(10): %d\n", R(10));*/);  // R(10) is $a0 
  INSTPAT("0000000 00000 00000 000 00000 11100 11", ecall  , N, s->dnpc = isa_raise_intr(11, s->pc));
  INSTPAT("0011000 00010 00000 000 00000 11100 11", mret   , N, exec_mret(); s->dnpc = cpu.csr.mepc);
  INSTPAT("??????? ????? ????? ??? ????? ????? ??", inv    , N, INV(s->pc));
  INSTPAT_END();

  R(0) = 0; // reset $zero to 0

  return 0;
}

int isa_exec_once(Decode *s) {
  s->isa.inst = inst_fetch(&s->snpc, 4);
  return decode_exec(s);
}
