import "DPI-C" function void stop_stimulation();

module top(
    input clk,
    input rst,
    input  [31:0] inst,
    output [31:0] pc,
    output [31:0] gpr0,
    output [31:0] gpr1,
    output [31:0] gpr2
);




wire [31:0]  imm;
wire [8:0]   inst_type;
ysyx_25020047_IDU u1(
    .clk(clk),
    .rst(rst),
    .reg_wen(reg_wen),
    .wdata(wdata),
    .dnpc(dnpc),
    .inst(inst),
    .imm(imm),
    .inst_type(inst_type),
    .rdata1(rdata1),
    .rdata2(rdata2),
    .pc(pc),
    .snpc(snpc),
    .gpr0(gpr0),
    .gpr1(gpr1),
    .gpr2(gpr2)
);


wire [31:0] result;
wire reg_wen;
wire [31:0] rdata1;
wire [31:0] rdata2;
ysyx_25020047_EXU u2(
    .inst_type(inst_type),
    .rdata1(rdata1),
    .rdata2(rdata2),
    .imm(imm),
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


