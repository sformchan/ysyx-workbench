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
    input pc_wen,
    input wdata,
    input  [31:0]       inst,
    output [11:0]       Iimm,
    output [8:0]   inst_type,
    output [31:0]     rdata1,
    output [31:0]         pc
);

// break down instruction
assign Ioc = inst[6:0];
assign Ifunct3 = inst[14:12];
assign Iimm = inst[31:20];
assign Irs1 = inst[19:15];
assign Ird = inst[11:7];


// joint the parts
wire [9:0] Itype = {Ifunct3, Ioc};



// judge the instruction type
assign inst_type = (Itype = 0000010011) ? 9'b000000001 : 9'b11111111; //addi
assign inst_type = (Itype = 0001100111) ? 9'b000000010 : 9'b11111111; //jalr
assign inst_type = (inst = 00000000000100000000000001110011) ? 9'b000000100 : 9'b111111111; //ebreak;

// add more instruction types as needed

// combine the signals




GPR u1
#(5, 32)
(
    .clk(clk),
    .rst(rst),
    .wen(wen),
    .raddr1(Irs1),
    .raddr2(),
    .wdata(wdata),
    .waddr(Ird),
    .rdata1(rdata1),
    .rdata2()
)
 

ysyx_25020047_PC u2
(
    .clk(clk),
    .rst(rst),
    .pc(pc)
);
endmodule

