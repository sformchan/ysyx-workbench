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
    input [6:0] oc,
    input [2:0] funct3,
    input [31:0] data1,
    input [11:0] imm,
    output reg [31:0] result,
    output wen
);

wire [31:0]  simm;
assign simm = $signed({20'b0, imm});

    always @(*)           
        begin
            result = 32'b0;
            wen = 1'b0;
            case (oc)
                7'b0010011: begin
                    case (funct3)
                        3'b000: begin
                            result = data1 + simm;
                            wen = 1;
                        end 
                        default: result = result;
                    endcase
                end
                7'b1110011: begin
                    case (funct3)
                        3'b000: begin
                            case (imm)
                                000000000001: begin
                                    wen = 0;
                                    stop_stimulation();
                                end
                                default: result = result;
                            endcase
                        end
                        default: result = result;
                    endcase
                end
                default: result = 32'b0;
            endcase 
        end                                          
                                                                   
                                                                   
endmodule
