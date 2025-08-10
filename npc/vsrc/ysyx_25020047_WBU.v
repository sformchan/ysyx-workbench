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
    input [63:0] inst_type,
    input [31:0] result,
    input [31:0] memdata,
	input [31:0] intr_mtvec,
	input [31:0] csr_rdata,
    input [31:0] snpc,
    output reg [31:0] wdata,
    output reg [31:0] dnpc    
);

    always @(*)           
        begin
            dnpc = snpc;                                       
            case(inst_type)
                64'h1: begin //addi
                    wdata = result; 
                end
                64'h2: begin //jalr
                    wdata = snpc;
                    dnpc = result;
                end
                64'h8: begin //add
                    //$display("wdata 0x%08x", wdata);
                    wdata = result;
                end
                64'h10: begin //lui
                    wdata = result; 
                end
                64'h20: begin //lw
                    wdata = memdata;
                end
                64'h40: begin //lbu
                    wdata = memdata;
                end
				64'h200: begin //auipc
					wdata = result;
				end
				64'h400: begin //jal
					wdata = snpc;
					dnpc = result;
				end
				64'h800: begin //sub
					wdata = result;
				end
				64'h1000: begin //slti
					wdata = result;
				end
				64'h2000: begin //sltiu
					wdata = result;
				end
				64'h4000: begin //beq
					dnpc = result;
				end
				64'h8000: begin //bne
					dnpc = result;
				end
				64'h10000: begin //slt
					wdata = result;
				end
				64'h20000: begin //sltu
					wdata = result;
				end
				64'h40000: begin //xor
					wdata = result;
				end
				64'h80000: begin //or
					wdata = result;
				end
				64'h100000: begin //and
					wdata = result;
				end
				64'h400000: begin //srai
					wdata = result;
				end
				64'h800000: begin //srli
					wdata = result;
				end
				64'h1000000: begin //slli
					wdata = result;
				end
				64'h2000000: begin //andi
					wdata = result;
				end
				64'h4000000: begin //ori
					wdata = result;
				end
				64'h8000000: begin //xori
					wdata = result;
				end
				64'h10000000: begin //blt
					dnpc = result;
				end
				64'h20000000: begin //bge
					dnpc = result;
				end
				64'h40000000: begin //bltu
					dnpc = result;
				end
				64'h80000000: begin //bgeu
					dnpc = result;
				end
				64'h100000000: begin //sll
					wdata = result;
				end
				64'h200000000: begin //slt
					wdata = result;
				end
				64'h400000000: begin //sltu
					wdata = result;
				end
				64'h800000000: begin //srl
					wdata = result;
				end
				64'h1000000000: begin //sra
					wdata = result;
				end
				64'h2000000000: begin //lb
                    wdata = memdata;
                end
				64'h4000000000: begin //lh
                    wdata = memdata;
                end
				64'h8000000000: begin //lhu
                    wdata = memdata;
                end
				64'h10000000000: begin //ecall
					dnpc = intr_mtvec;
					//$display("switch to mtvec: 0x%08x", dnpc);
				end
				64'h20000000000: begin //csrrw
					wdata = csr_rdata;
				end
				64'h40000000000: begin //csrrs
					wdata = csr_rdata;
				end
                default: wdata = 32'b0;
            endcase
        end                                          
                                                                   
endmodule