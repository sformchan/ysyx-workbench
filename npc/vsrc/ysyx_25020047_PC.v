module ysyx_25020047_PC(
    input clk,
    input rst, 
    input [31:0] dnpc,
    output reg [31:0] pc,
    output [31:0] snpc
);

assign snpc = pc + 32'h4; 

always @(posedge clk) begin
    if(rst) pc <= 32'h80000000;
    else pc <= dnpc;
end

always @(*) begin
	set_gpr(32, pc);
end
endmodule
