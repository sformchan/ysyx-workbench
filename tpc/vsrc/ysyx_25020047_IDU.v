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
// Created by:             Please Write You Name 
// Created date:           2025/03/11 22:50:48
// mail      :             Please Write mail 
// Version:                V1.0
// TEXT NAME:              npc_dc.v
// PATH:                   ~/ysyx-workbench/npc/vsrc/npc_dc.v
// Descriptions:           
//                         
//----------------------------------------------------------------------------------------
//****************************************************************************************//

module ysyx_25020047_IDU(
    input clk,
    input rst,
    input reg_wen,
    input  [31:0]      wdata,
    input  [31:0]       dnpc,
    input  [31:0]       inst,
    output reg [31:0]       imm,
    output reg [8:0]   inst_type,
    output [31:0]     rdata1,
    output [31:0]     rdata2,
    output [31:0]         pc,
    output [31:0]       snpc,
    output [31:0]  gpr0,
    output [31:0]  gpr1,
    output [31:0]  gpr2
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
wire [31:0] sIimm;
assign sIimm = {{20{Iimm[11]}}, Iimm}; // sign-extend the immediate value

// Rtype
wire [4:0] Rrs1;
wire [4:0] Rrs2;
wire [4:0] Rrd;
assign Rrd = inst[11:7];
assign Rrs1 = inst[19:15];
assign Rrs2 = inst[24:20];

//Utype
wire [4:0] Urd;
wire [19:0] Uimm;
assign Urd = inst[11:7];
assign Uimm = inst[31:12];
wire [31:0] eUimm;
assign eUimm = {Uimm, 12'b0}; // zero-extend the immediate value

// combine the signals
wire [4:0] rs1;
wire [4:0] rs2;
wire [4:0] rd;
assign rs1 = Rrs1 | Irs1;
assign rs2 = Rrs2;
assign rd = Rrd | Ird | Urd;

// add more instruction types as needed  //judge the type of instruction
    always @(*)           
        begin
            casez(inst)
                32'b?????????????????000?????0010011: inst_type = 9'b000000001; // addi
                32'b?????????????????000?????1100111: inst_type = 9'b000000010; // jalr
                32'b00000000000100000000000001110011: inst_type = 9'b000000100; // ebreak
                32'b0000000??????????000?????0110011: inst_type = 9'b000001000; // add
                32'b?????????????????????????0110111: inst_type = 9'b000010000; // lui
                default:                              inst_type = 9'b000000000; // default case
            endcase                                     
        end                                          

// judge the imm
        always @(*)           
            begin                                        
                case(inst_type)
                    9'b000000001: imm = sIimm; // addi
                    9'b000000010: imm = sIimm; // jalr
                    9'b000010000: imm = eUimm; // lui
                    default:      imm = 32'b0; // default case
                endcase
            end                                          


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
    .gpr0(gpr0),
    .gpr1(gpr1),
    .gpr2(gpr2)
);
 

ysyx_25020047_PC u2
(
    .clk(clk),
    .rst(rst),
    .dnpc(dnpc),
    .pc(pc),
    .snpc(snpc)
);

endmodule

