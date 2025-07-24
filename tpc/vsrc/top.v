import "DPI-C" function void stop_stimulation();

module top(
    input clk,
    input rst,
    input  [31:0] inst,
    output [31:0] pc 
);




wire [11:0]   Iimm;
wire [8:0]   inst_type;
ysyx_25020047_IDU u0(
    .clk(clk),
    .rst(rst),
    .reg_wen(reg_wen),
    .wdata(wdata),
    .dnpc(dnpc),
    .inst(inst),
    .Iimm(Iimm),
    .inst_type(inst_type),
    .rdata1(rdata1),
    .pc(pc),
    .snpc(snpc)
);


wire [31:0] result;
wire reg_wen;
wire [31:0] rdata1;
wire pc_wen;
ysyx_25020047_EXU u2(
    .inst_type(inst_type),
    .rdata1(rdata1),
    .imm(Iimm),
    .result(result),
    .reg_wen(reg_wen)
);


wire [31:0]  wdata;
wire [31:0]  dnpc;
wire [31:0]  snpc;
ysyx_25020047_WBU u3(
    .inst_type (inst_type),
    .result    (result),
    .snpc      (snpc),
    .wdata     (wdata),
    .dnpc      (dnpc)
);

endmodule


