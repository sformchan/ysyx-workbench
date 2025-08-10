`timescale 1ns / 1ps
//****************************************VSCODE PLUG-IN**********************************//
//----------------------------------------------------------------------------------------
// IDE :                   VSCODE     
// VSCODE plug-in version: Verilog-Hdl-Format-3.5.20250220
// VSCODE plug-in author : Jiang Percy
//----------------------------------------------------------------------------------------
//****************************************Copyright (c)***********************************//
// Copyright(C)            Please Write Company name
// All rights reserved     
// File name:              
// Last modified Date:     2025/03/11 22:50:48
// Last Version:           V1.0
// Descriptions:           
//----------------------------------------------------------------------------------------
// Created by:             Sform Chan 
// Created date:           2025/03/11 22:50:48
// mail      :             Please Write mail 
// Version:                V1.0
// TEXT NAME:              
// PATH:                   tpc
// Descriptions:           
//                         
//----------------------------------------------------------------------------------------
//****************************************************************************************//

module ysyx_25020047_IDU(
    input clk,
    input rst,
    input reg_wen,
	input csr_wen,
	input intr,
    input  [31:0]      wdata,
    input  [31:0]       dnpc,
    input  [31:0]       inst,
    output reg [31:0]       imm,
    output reg [63:0]   inst_type,
    output [31:0]     rdata1,
    output [31:0]     rdata2,
    output [31:0]         pc,
    output [31:0]       snpc,
	output [4:0] shamt,
	output [31:0] intr_mtvec,
	output [31:0] csr_rdata
);



// break down instruction

wire [6:0] opcode;
assign opcode = inst[6:0];

// Itype    
wire [4:0] Irs1;
wire [4:0] Ird;
wire [11:0] Iimm;
assign Iimm = inst[31:20];
assign Irs1 = inst[19:15];
assign Ird = inst[11:7];
assign shamt = inst[24:20];
wire [31:0] sIimm;
assign sIimm = {{20{Iimm[11]}}, Iimm}; // sign-extend the immediate value

// Rtype
wire [4:0] Rrs1;
wire [4:0] Rrs2;
wire [4:0] Rrd;
assign Rrd = inst[11:7];
assign Rrs1 = inst[19:15];
assign Rrs2 = inst[24:20];

// Utype
wire [4:0] Urd;
wire [19:0] Uimm;
assign Urd = inst[11:7];
assign Uimm = inst[31:12];
wire [31:0] zUimm;
assign zUimm = {Uimm, 12'b0}; // zero-extend the immediate value

// Stype
wire [4:0] Srs1;
wire [4:0] Srs2;
wire [11:0] Simm;
assign Srs1 = inst[19:15];
assign Srs2 = inst[24:20];
assign Simm = {inst[31:25], inst[11:7]};
wire [31:0] sSimm;
assign sSimm = {{20{Simm[11]}}, Simm};

// Jtype
wire [4:0] Jrd;
wire [20:0] Jimm;
assign Jimm = {inst[31], inst[19:12], inst[20], inst[30:21], 1'b0};
wire [31:0] sJimm;
assign sJimm = {{11{Jimm[20]}}, Jimm};

// Btype
wire [4:0] Brs1;
wire [4:0] Brs2;
wire [12:0] Bimm;
assign Brs1 = inst[19:15];
assign Brs2 = inst[24:20];
assign Bimm = {inst[31], inst[7], inst[30:25], inst[11:8], 1'b0};
wire [31:0] sBimm;
assign sBimm = {{19{Bimm[12]}}, Bimm};

// combine the signals
wire [4:0] rs1;
wire [4:0] rs2;
wire [4:0] rd;
assign rs1 = Rrs1 | Irs1 | Srs1 | Brs1;
assign rs2 = Rrs2 | Srs2 | Brs2;
assign rd = Rrd | Ird | Urd | Jrd;

// csr
wire [31:0] csr_raddr = {20'b0, Iimm};
reg	 [31:0] csr_wdata;
always @(*) begin
	case(inst_type)
		64'h20000000000: csr_wdata = rdata1;
		64'h40000000000: csr_wdata = csr_rdata | rdata1;
		default: csr_wdata = 32'b0;
	endcase
end

// add more instruction types as needed  //judge the type of instruction
    always @(*)           
        begin
            casez(inst)
			//U-type
				32'b?????????????????????????0110111: inst_type = 64'h10;  // lui
				32'b?????????????????????????0010111: inst_type = 64'h200; // auipc

			//J-type
				32'b?????????????????????????1101111: inst_type = 64'h400; // jal

			//I-type	
                32'b?????????????????000?????0010011: inst_type = 64'h1; // addi
                32'b?????????????????000?????1100111: inst_type = 64'h2; // jalr
				32'b?????????????????010?????0000011: inst_type = 64'h20; // lw
                32'b?????????????????100?????0000011: inst_type = 64'h40; // lbu
				32'b?????????????????010?????0010011: inst_type = 64'h1000; // slti
				32'b?????????????????011?????0010011: inst_type = 64'h2000; // sltiu
				32'b0100000??????????101?????0010011: inst_type = 64'h400000; // srai
				32'b0000000??????????101?????0010011: inst_type = 64'h800000; // srli
				32'b0000000??????????001?????0010011: inst_type = 64'h1000000; // slli
				32'b?????????????????111?????0010011: inst_type = 64'h2000000; // andi
				32'b?????????????????110?????0010011: inst_type = 64'h4000000; // ori
				32'b?????????????????100?????0010011: inst_type = 64'h8000000; // xori
				32'b?????????????????000?????0000011: inst_type = 64'h2000000000; // lb
				32'b?????????????????001?????0000011: inst_type = 64'h4000000000; // lh
				32'b?????????????????101?????0000011: inst_type = 64'h8000000000; // lhu

				32'b?????????????????001?????1110011: inst_type = 64'h20000000000; // csrrw
				32'b?????????????????010?????1110011: inst_type = 64'h40000000000; // csrrs
				
			//B-type
				32'b?????????????????000?????1100011: inst_type = 64'h4000; // beq
				32'b?????????????????001?????1100011: inst_type = 64'h8000; // bne
				32'b?????????????????100?????1100011: inst_type = 64'h10000000; // blt
				32'b?????????????????101?????1100011: inst_type = 64'h20000000; // bge
				32'b?????????????????110?????1100011: inst_type = 64'h40000000; // bltu
				32'b?????????????????111?????1100011: inst_type = 64'h80000000; // bgeu

			//R-type
                32'b0000000??????????000?????0110011: inst_type = 64'h8; // add
				32'b0100000??????????000?????0110011: inst_type = 64'h800; // sub
				32'b0000000??????????010?????0110011: inst_type = 64'h10000; // slt
				32'b0000000??????????011?????0110011: inst_type = 64'h20000; // sltu
				32'b0000000??????????100?????0110011: inst_type = 64'h40000; // xor
				32'b0000000??????????110?????0110011: inst_type = 64'h80000; // or
				32'b0000000??????????111?????0110011: inst_type = 64'h100000; //and 
				32'b0000000??????????001?????0110011: inst_type = 64'h100000000; //sll
				32'b0000000??????????010?????0110011: inst_type = 64'h200000000; //slt
				32'b0000000??????????011?????0110011: inst_type = 64'h400000000; //sltu
				32'b0000000??????????101?????0110011: inst_type = 64'h800000000; //srl
				32'b0100000??????????101?????0110011: inst_type = 64'h1000000000; //sll
                
			//S-type
                32'b?????????????????010?????0100011: inst_type = 64'h80; // sw
                32'b?????????????????000?????0100011: inst_type = 64'h100; // sb
				32'b?????????????????001?????0100011: inst_type = 64'h200000; // sh

			//N-type
				32'b00000000000100000000000001110011: inst_type = 64'h4; // ebreak
				32'b00000000000000000000000001110011: inst_type = 64'h10000000000; // ecall

                default:                              inst_type = 64'hFFFFFFFFFFFFFFFF; // default case
            endcase                                     
        end                                          

// judge the imm
        always @(*)           
            begin      
				imm = 0;                                  
                case(inst_type)
                    64'h1: imm = sIimm; // addi
                    64'h2: imm = sIimm; // jalr
					64'h20: imm = sIimm; // lw
                    64'h40: imm = sIimm; // lbu
					64'h1000: imm = sIimm; // slti
					64'h2000: imm = sIimm; // sltiu
					64'h2000000: imm = sIimm; // andi
					64'h4000000: imm = sIimm; // ori
					64'h8000000: imm = sIimm; // xori
					64'h2000000000: imm = sIimm; // lb
					64'h4000000000: imm = sIimm; // lh
					64'h8000000000: imm = sIimm; // lhu

                    64'h10: imm = zUimm; // lui
					64'h200: imm = zUimm; // auipc

                    64'h80: imm = sSimm; // sw
                    64'h100: imm = sSimm; // sb
					64'h200000: imm = sSimm; // sh

					64'h4000: imm = sBimm; // beq
					64'h8000: imm = sBimm; // bne
					64'h10000000: imm = sBimm; // blt
					64'h20000000: imm = sBimm; // bge
					64'h40000000: imm = sBimm; // bltu
					64'h80000000: imm = sBimm; // bgeu

					64'h400: imm = sJimm; // jal

                    default:      imm = 32'b0; // default case
                endcase
            end                                          

wire [31:0] NO;
GPR #(5, 32) u1
(
    .clk(clk),
    .rst(rst),
    .wen(reg_wen),
    .raddr1(rs1),
    .raddr2(rs2),
    .wdata(wdata),
    .waddr(rd),
    .rdata1(rdata1),
    .rdata2(rdata2),
	.NO(NO)
    // .gpr0(gpr0),
    // .gpr1(gpr1),
    // .gpr2(gpr2)
);
 

CSR #(32) u2
(
	.clk(clk),
	.rst(rst),
	.wen(csr_wen),
	.addr(csr_raddr),
	.csr_wdata(csr_wdata),
	.csr_rdata(csr_rdata),
	.intr(intr),
	.intr_NO(NO),
	.intr_epc(pc),
	.intr_mtvec(intr_mtvec)
);


ysyx_25020047_PC u3
(
    .clk(clk),
    .rst(rst),
    .dnpc(dnpc),
    .pc(pc),
    .snpc(snpc)
);

endmodule

