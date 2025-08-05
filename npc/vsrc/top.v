import "DPI-C" function void set_npc_state(input int state);
import "DPI-C" function int pmem_read(input int raddr, input int flag);
import "DPI-C" function void pmem_write(input int waddr, input int wdata, input int wmask);
import "DPI-C" function void set_gpr(input int i, input int val);

module top(
    input clk,
    input rst,
    //input  [31:0] inst,
    output [31:0] pc,
    output [31:0] gpr0,
    output [31:0] gpr1,
    output [31:0] gpr2,
	output [31:0] dnpc
);

wire [31:0] inst;
ysyx_25020047_IFU u0(
    .pc(pc),
    .inst(inst)
);


wire [31:0]  imm;
wire [31:0]   inst_type;
wire [4:0] shamt;
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
    .gpr2(gpr2),
	.shamt(shamt)
);


wire [31:0] result;
wire reg_wen;
wire [31:0] rdata1;
wire [31:0] rdata2;
wire read;
wire write;
ysyx_25020047_EXU u2(
    .inst_type(inst_type),
    .rdata1(rdata1),
    .rdata2(rdata2),
    .imm(imm),
	.shamt(shamt),
	.pc(pc),
	.snpc(snpc),
    .result(result),
    .reg_wen(reg_wen),
    .read(read),
    .write(write)
);


wire [31:0] memdata;
ysyx_25020047_LSU u3(
    .inst_type(inst_type),
    .raddr(result),
    .waddr(result),
    .wdata(rdata2),
    .read(read),
    .write(write),
    .memdata(memdata)
);


wire [31:0]  wdata;
//wire [31:0]  dnpc;
wire [31:0]  snpc;
ysyx_25020047_WBU u4(
    .inst_type (inst_type),
    .result    (result),
    .memdata (memdata),
    .snpc      (snpc),
    .wdata     (wdata),
    .dnpc      (dnpc)
);




endmodule
