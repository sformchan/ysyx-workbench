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
    input  [31:0]   inst,
    output [6:0]     Ioc,
    output [2:0] Ifunct3,
    output [11:0]   Iimm,
    output [4:0]    Irs1,
    output [4:0]     Ird                         
);

assign Ioc = inst[6:0];
assign Ifunct3 = inst[14:12];
assign Iimm = inst[31:20];
assign Irs1 = inst[19:15];
assign Ird = inst[11:7];
                                                                   
endmodule
