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
// Last modified Date:     2025/07/24 13:48:30
// Last Version:           V1.0
// Descriptions:           
//----------------------------------------------------------------------------------------
// Created by:             Please Write You Name 
// Created date:           2025/07/24 13:48:30
// mail      :             Please Write mail 
// Version:                V1.0
// TEXT NAME:              WBU.v
// PATH:                   ~/ysyx-workbench/tpc/vsrc/WBU.v
// Descriptions:           
//                         
//----------------------------------------------------------------------------------------
//****************************************************************************************//

module ysyx_25020047_WBU(
    input [8:0] inst_type,
    input [31:0] result,
    input [31:0] memdata,
    input [31:0] snpc,
    output reg [31:0] wdata,
    output reg [31:0] dnpc    
);

    always @(*)           
        begin
            dnpc = snpc;                                       
            case(inst_type)
                9'b000000001: begin //addi
                    wdata = result; 
                end
                9'b000000010: begin //jalr
                    wdata = snpc;
                    dnpc = result;
                end
                9'b000001000: begin
                    $display("wdata 0x%08x", wdata);
                    wdata = result;
                end
                9'b000010000: begin //lui
                    wdata = result; 
                end
                9'b000100000: begin //lw
                    wdata = memdata;
                end
                9'b001000000: begin //lbu
                    wdata = memdata;
                end
                default: wdata = 32'b0;
            endcase
        end                                          
                                                                   
endmodule