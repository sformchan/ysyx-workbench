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
    input [31:0]  inst_type,
    input [31:0] rdata1,
    input [31:0] rdata2,
    input [31:0] imm,
	input [31:0] pc,
	input [31:0] snpc,
    output reg [31:0] result,
    output reg reg_wen,
    output reg read,
    output reg write
);



    always @(*)           
        begin
            read = 1'b0;
            write = 1'b0;
            reg_wen = 1'b0;
            case(inst_type)
                32'h1: begin //addi
                    result = rdata1 + imm;
                    reg_wen = 1'b1;
                end
                32'h2: begin //jalr
                    result = (rdata1 + imm) & ~1;
                    reg_wen = 1'b1;
                end
                32'h4: begin //ebreak
                    reg_wen = 1'b0; // ebreak does not write back
					set_npc_state(32'h2); // end simulation
                end
                32'h8: begin //add
                    //$display("rdata1 0x%08x | rdata2 0x%08x | result 0x%08x", rdata1, rdata2, result);
                    result = rdata1 + rdata2; // R-type instruction
                    reg_wen = 1'b1;
                end
                32'h10: begin //lui
                    result = imm;
                    reg_wen = 1'b1; 
                end
                32'h20: begin //lw
                    // $display("rdata1 imm 0x%08x 0x%08x", rdata1, imm);
                    result = rdata1 + imm;
                    // $display("result 0x%08x", result);
                    reg_wen = 1'b1;
                    read = 1'b1;
                end
                32'h40: begin //lbu
                    result = rdata1 + imm;
                    reg_wen = 1'b1;
                    read = 1'b1;
                end
                32'h80: begin //sw
                    result = rdata1 + imm;
                    write = 1'b1;
                end
                32'h100: begin //sb
                    // $display("result 0x%08x", result);
                    result = rdata1 + imm;
                    write = 1'b1;
                end
				32'h200: begin //auipc
					result = pc + imm;
					reg_wen = 1'b1;
				end
				32'h400: begin //jal
					result = pc + imm;
					reg_wen = 1'b1;
				end
				32'h800: begin //sub
					result = rdata1 - rdata2;
					reg_wen = 1'b1;
				end
				32'h1000: begin //stli
					result = ($signed(rdata1) < $signed(imm)) ? 32'b1 : 32'b0;
					reg_wen = 1'b1;
				end
				32'h2000: begin //stliu
					result = ($unsigned(rdata1) < $unsigned(imm)) ? 32'b1 : 32'b0;
					reg_wen = 1'b1;
				end
				32'h4000: begin //beq
					result = (rdata1 == rdata2) ? (pc + imm) : snpc;
				end
				32'h8000: begin //bne
					result = (rdata1 != rdata2) ? (pc + imm) : snpc;
				end
				32'h10000: begin //slt
					result = ($signed(rdata1) < $signed(rdata2)) ? 32'b1 : 32'b0;
					reg_wen = 1'b1;
				end
				32'h20000: begin //sltu
					result = ($unsigned(rdata1) < $unsigned(rdata2)) ? 32'b1 : 32'b0;
					reg_wen = 1'b1;
				end
				32'h40000: begin //xor
					result = rdata1 ^ rdata2;
					reg_wen = 1'b1;
				end
				32'h80000: begin //or
					result = rdata1 | rdata2;
					reg_wen = 1'b1;
				end
				32'h100000: begin //and
					result = rdata1 & rdata2;
					reg_wen = 1'b1;
				end
                default: begin
					set_npc_state(32'h4); // abort simulation
					result = 32'b0; // default case
				end
            endcase
        end                                          
                                                                   
                                                                   
endmodule
