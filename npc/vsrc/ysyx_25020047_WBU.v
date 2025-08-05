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
    input [31:0] inst_type,
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
                32'h1: begin //addi
                    wdata = result; 
                end
                32'h2: begin //jalr
                    wdata = snpc;
                    dnpc = result;
                end
                32'h8: begin //add
                    //$display("wdata 0x%08x", wdata);
                    wdata = result;
                end
                32'h10: begin //lui
                    wdata = result; 
                end
                32'h20: begin //lw
                    wdata = memdata;
                end
                32'h40: begin //lbu
                    wdata = memdata;
                end
				32'h200: begin //auipc
					wdata = result;
				end
				32'h400: begin //jal
					wdata = snpc;
					dnpc = result;
				end
				32'h800: begin //sub
					wdata = result;
				end
				32'h1000: begin //slti
					wdata = result;
				end
				32'h2000: begin //sltiu
					wdata = result;
				end
				32'h4000: begin //beq
					dnpc = result;
				end
				32'h8000: begin //bne
					dnpc = result;
				end
				32'h10000: begin //slt
					wdata = result;
				end
				32'h20000: begin //sltu
					wdata = result;
				end
				32'h40000: begin //xor
					wdata = result;
				end
				32'h80000: begin //or
					wdata = result;
				end
				32'h100000: begin //and
					wdata = result;
				end
                default: wdata = 32'b0;
            endcase
        end                                          
                                                                   
endmodule