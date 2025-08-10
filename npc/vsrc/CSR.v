module CSR #(DATA_WIDTH = 1) (
  input                    clk,
  input                    rst,
  input                    wen,
  input  [DATA_WIDTH-1:0]  addr, //imm
  input  [DATA_WIDTH-1:0]  csr_wdata, 
  output reg [DATA_WIDTH-1:0]  csr_rdata,
  input                    intr,
  input   [DATA_WIDTH - 1 : 0] intr_NO,
  input   [DATA_WIDTH - 1 : 0] intr_epc,
  output  [DATA_WIDTH - 1 : 0] intr_mtvec
);
  reg [DATA_WIDTH-1:0] mepc;
  reg [DATA_WIDTH-1:0] mstatus;
  reg [DATA_WIDTH-1:0] mtvec;
  reg [DATA_WIDTH-1:0] mcause;

assign intr_mtvec = mtvec;

always @(*) begin
	case(addr)
		32'h305: csr_rdata = mtvec;
		32'h341: csr_rdata = mepc;
		32'h342: csr_rdata = mcause;
		32'h300: csr_rdata = mstatus;
		default: ;
	endcase
end
  
wire mie_bit = mstatus[3];
  always @(posedge clk) begin
    if (rst) begin
      mepc <= {DATA_WIDTH{1'b0}};
	  mstatus <= {DATA_WIDTH{1'b0}};
	  mtvec <= {DATA_WIDTH{1'b0}};
	  mcause <= {DATA_WIDTH{1'b0}};
    end
    else if (wen && addr != 32'b0) begin
		$display("0x%08x", csr_data);
		$display("0x%08x", addr);	
		case(addr) 
			32'h305: mtvec <= csr_wdata;
			32'h341: mepc <= csr_wdata;
			32'h342: mcause <= csr_wdata;
			32'h300: mstatus <= csr_wdata;
			default: ;
		endcase
	end
	else if (intr) begin
		
		
		mepc <= intr_epc;
        mcause <= intr_NO;
        // mstatus 更新 (MPIE <= MIE; MIE<=0; MPP <= intr_priv) 
    	mstatus[7] <= mie_bit;            // MPIE = 原来的 MIE
    	mstatus[3] <= 1'b0;               // MIE = 0
    	mstatus[12:11] <= 2'b11;          // MPP = Machine
	end 
  end

  always @(*) begin
	set_csr(mepc, mtvec, mcause, mstatus);
  end
endmodule


  