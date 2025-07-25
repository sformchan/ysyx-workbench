`timescale 1ns / 1ps
//****************************************VSCODE PLUG-IN**********************************//
//----------------------------------------------------------------------------------------
// IDE :                   VSCODE     
// VSCODE plug-in version: Verilog-Hdl-Format-3.6.20250620
// VSCODE plug-in author : Jiang Percy
//----------------------------------------------------------------------------------------
//****************************************Copyright (c)***********************************//
// Copyright(C)            Please Write Company name
// All rights reserved     
// File name:              
// Last modified Date:     2025/07/24 21:04:00
// Last Version:           V1.0
// Descriptions:           
//----------------------------------------------------------------------------------------
// Created by:             Please Write You Name 
// Created date:           2025/07/24 21:04:00
// mail      :             Please Write mail 
// Version:                V1.0
// TEXT NAME:              ysyx_25020047_IFU.v
// PATH:                   ~/ysyx-workbench/tpc/vsrc/ysyx_25020047_IFU.v
// Descriptions:           
//                         
//----------------------------------------------------------------------------------------
//****************************************************************************************//

module ysyx_25020047_IFU(
    input [31:0] pc,
    output [31:0] inst                  
);

assign inst = read_inst(pc); 
                                                                   
endmodule