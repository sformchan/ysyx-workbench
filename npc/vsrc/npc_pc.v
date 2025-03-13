module pc(
    input clk,
    input rst,
    output reg [31:0] pc
);

initial begin
    pc = 32'h80000000;
end

always @(posedge clk or posedge rst) begin
    if(rst) pc <= 32'h80000000;
    else pc <= 32'h4;    
end


endmodule