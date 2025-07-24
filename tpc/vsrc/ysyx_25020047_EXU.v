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
// Last modified Date:     2025/03/11 23:36:49
// Last Version:           V1.0
// Descriptions:           
//----------------------------------------------------------------------------------------
// Created by:             Please Write You Name 
// Created date:           2025/03/11 23:36:49
// mail      :             Please Write mail 
// Version:                V1.0
// TEXT NAME:              npc_aul.v
// PATH:                   ~/ysyx-workbench/npc/vsrc/npc_aul.v
// Descriptions:           
//                         
//----------------------------------------------------------------------------------------
//****************************************************************************************//



module ysyx_25020047_EXU(
    input [7:0]  inst_type,
    input [31:0] rdata1,
    input [11:0] imm,
    output reg [31:0] result,
    output reg reg_wen,
    output reg pc_wen
);

wire [31:0]  simm;
assign simm = {{20{imm[11]}}, imm}; // sign-extend the immediate value

    always @(*)           
        begin
            wen = 1'b0;
            case(inst_type)
                8'b000000001: begin //addi
                    result = rdata1 + simm;
                    reg_wen = 1'b1;
                end
                8'b000000010: begin //jalr
                    result = (rdata1 + simm) & ~1;
                    reg_wen = 1'b1;
                    pc_wen = 1'b1; // jalr updates the PC
                end
                8'b000000100: begin //ebreak
                    reg_wen = 1'b0; // ebreak does not write back
                    pc_wen = 1'b0; // ebreak does not update the PC
                    stop_stimulation(); // call DPI-C function to stop simulation
                end
                default: result = 32'b0; // default case
            endcase
        end                                          
                                                                   
                                                                   
endmodule
