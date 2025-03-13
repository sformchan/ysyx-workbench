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

module npc_aul(
    input [6:0] oc,
    input [2:0] funct3,
    input [31:0] data1,
    input [31:0] imm,
    output reg [31:0] result
);

    always @(*)           
        begin
            result = 32'b0;
            case (oc)
                7'b0010011: begin
                    case (funtc3)
                        3'b000: result = data1 + imm; 
                        default: result = result;
                    endcase
                end
                default: result = 32'b0;
            endcase 
        end                                          
                                                                   
                                                                   
endmodule