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
// Last modified Date:     2025/03/11 23:11:29
// Last Version:           V1.0
// Descriptions:           
//----------------------------------------------------------------------------------------
// Created by:             Please Write You Name 
// Created date:           2025/03/11 23:11:29
// mail      :             Please Write mail 
// Version:                V1.0
// TEXT NAME:              npc_gpr.v
// PATH:                   ~/ysyx-workbench/npc/vsrc/npc_gpr.v
// Descriptions:           
//                         
//----------------------------------------------------------------------------------------
//****************************************************************************************//

module npc_gpr(
    input clk,
    input rst,
    input wen,
    input [4:0] raddr1,
    input [4:0] waddr,
    output [31:0] rdata1,
    input  [31:0] wdata
);

    reg [31:0] gpr [31:0];
    assign rdata = gpr[raddr];
    initial begin
        gpr
    end
    always @(posedge clk)           
        begin
            gpr[0] <= 32'h00000000;                                    
            if (wen && waddr != 5'h00000) begin
                gpr[waddr] <= wdata;
            end
            else gpr[waddr] <= gpr[waddr];                                     
        end                                          
                                                                   
endmodule