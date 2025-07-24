module ysyx_25020047_PC(
    input clk,
    input rst, 
    input pc_wen,
    input [31:0] dnpc,
    output reg [31:0] pc,
    output [31:0] snpc
);

assign snpc = pc + 32'h4; 

always @(posedge clk) begin
    if(rst) pc <= 32'h80000000;
    else if(pc_wen) pc <= dnpc; // update PC if pc_wen is asserted  
end


endmodule
