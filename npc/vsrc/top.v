import "DPI-C" function void stop_stimulation();

module top(
    input clk,
    input rst,
    output [31:0] pc,
    output [31:0] gpr0, gpr1, gpr2,
    input [31:0] inst
);

//internal variables

  //decoder
wire [6:0]     Ioc;
wire [2:0] Ifunct3;
wire [11:0]   Iimm;
wire [4:0]    Irs1;   //raddr1
wire [4:0]     Ird;   //waddr
  //gpr
wire [31:0] rdata1;
wire [31:0]  wdata;
wire           wen;


ysyx_25020047_PC u0(
    .clk (clk ),
    .rst (rst ),
    .pc  (pc  )
);

ysyx_25020047_IDU u1(
    .inst       (inst),
    .Ioc         (Ioc),
    .Ifunct3 (Ifunct3),
    .Iimm       (Iimm),
    .Irs1       (Irs1),
    .Ird         (Ird)
);

ysyx_25020047_GPR u2(
    .clk        (clk),
    .wen        (wen),
    .raddr1    (Irs1),
    .waddr      (Ird),
    .rdata1  (rdata1),
    .gpr0      (gpr0),
    .gpr1      (gpr1),
    .gpr2      (gpr2),
    .wdata    (wdata)
);

ysyx_25020047_EXU u3(
    .oc         (Ioc),
    .funct3 (Ifunct3),
    .data1   (rdata1),
    .imm       (Iimm),
    .result   (wdata),
    .wen        (wen)
);



endmodule


