module ysyx_25020047_PC(
    input clk,
    input rst,
    output reg [31:0] pc
);


always @(posedge clk) begin
    if(rst) pc <= 32'h80000000;
    else pc <= pc + 32'h4;            
end


endmodule
