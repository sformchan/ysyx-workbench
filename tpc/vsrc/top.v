import "DPI-C" function void stop_stimulation();

module top(
    input clk,
    input rst,
    input  [31:0] inst,
    output [31:0] pc,    
);




wire [11:0]   Iimm;
wire [7:0]   inst_type;
ysyx_25020047_IDU u1(
    .clk       (clk       ),
    .rst       (rst       ),
    .reg_wen   (wen       ),
    .pc_wen    (pc_wen    ),
    .wdata     (wdata     ),
    .inst      (inst      ),
    .Iimm      (Iimm      ),
    .inst_type (inst_type ),
    .rdata1    (rdata1    )
);


wire [31:0] result;
wire reg_wen;
wire [31:0] rdata1;
wire pc_wen;
ysyx_25020047_EXU u2(
    .oc         (Ioc),
    .funct3 (Ifunct3),
    .rdata1   (rdata1),
    .imm       (Iimm),
    .result  (result),
    .reg_wen (reg_wen),
    .pc_wen  (pc_wen)
);


wire [31:0]  wdata;
ysyx_25020047_WBU u3(
    .inst_type (inst_type),
    .result    (result),
    .snpc      (snpc),
    .wdata     (wdata),
    .dnpc      (dnpc)
);

endmodule


