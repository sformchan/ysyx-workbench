module GPR #(ADDR_WIDTH = 1, DATA_WIDTH = 1) (
  input                    clk,
  input                    rst,
  input                    wen,
  input  [ADDR_WIDTH-1:0]  raddr1, //rs1
  input  [ADDR_WIDTH-1:0]  raddr2, //rs2
  input  [DATA_WIDTH-1:0]  wdata, 
  input  [ADDR_WIDTH-1:0]  waddr, //rd
  output [DATA_WIDTH-1:0]  rdata1,
  output [DATA_WIDTH-1:0]  rdata2,
  output [DATA_WIDTH-1:0]  NO
//   output [DATA_WIDTH-1:0]  gpr0,
//   output [DATA_WIDTH-1:0]  gpr1,
//   output [DATA_WIDTH-1:0]  gpr2
);
  reg [DATA_WIDTH-1:0] rf [2**ADDR_WIDTH-1:0];

  assign rdata1 = rf[raddr1];  
  assign rdata2 = rf[raddr2];  

  //test 
//   assign gpr0 = rf[5'b00000]; // x0
//   assign gpr1 = rf[5'b01000]; // x1
//   assign gpr2 = rf[5'b01100]; // x2

assign NO = rf[5'b01111];


  
  always @(posedge clk) begin
    if (rst) begin
      integer i; //this for loop is actually not a loop,
      for (i = 0; i < 2**ADDR_WIDTH; i = i + 1) begin  
        rf[i] <= {DATA_WIDTH{1'b0}}; //it just initializes all registers to zero at the same time
      end
    end
    else if (wen && waddr != 5'b0) begin
		rf[waddr] <= wdata; 
	end 
  end

  always @(*) begin
	integer i;
	for(i = 0; i < 16; i++) set_gpr(i, rf[i]);
  end
endmodule

