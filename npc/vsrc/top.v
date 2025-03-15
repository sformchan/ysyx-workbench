module top(
    input clk,
    input rst,
    input wen,
    output reg [31:0] pc,
    input [31:0] inst
);

//internal variables
  //pc
//reg  [31:0]     pc;
  //decoder
//wire [31:0]   inst;
wire [6:0]     Ioc;
wire [2:0] Ifunct3;
wire [11:0]   Iimm;
wire [4:0]    Irs1;
wire [4:0]     Ird;
  //gpr
wire [31:0] rdata1;
wire [31:0]  wdata;

//wire           wen;
  //aul
wire [31:0]  sIimm;
assign sIimm = $signed({20'b0, Iimm});


npc_pc u0(
    .clk (clk ),
    .rst (rst ),
    .pc  (pc  )
);

npc_dc u1(
    .inst       (inst),
    .Ioc         (Ioc),
    .Ifunct3 (Ifunct3),
    .Iimm       (Iimm),
    .Irs1       (Irs1),
    .Ird         (Ird)
);

npc_gpr u2(
    .clk        (clk),
    .wen        (wen),
    .raddr1    (Irs1),
    .waddr      (Ird),
    .rdata1  (rdata1),
    .wdata    (wdata)
);

npc_aul u3(
    .oc         (Ioc),
    .funct3 (Ifunct3),
    .data1   (rdata1),
    .imm      (sIimm),
    .result   (wdata)
);



endmodule


