module CSR #(DATA_WIDTH = 1) (
  input                    clk,
  input                    rst,
  input					   start,
  input                    wen,
  input  [DATA_WIDTH-1:0]  addr, //imm
  input  [DATA_WIDTH-1:0]  csr_wdata, 
  output reg [DATA_WIDTH-1:0]  csr_rdata,
  input                    intr,
  input   [DATA_WIDTH - 1 : 0] intr_NO,
  input   [DATA_WIDTH - 1 : 0] intr_epc,
  output  [DATA_WIDTH - 1 : 0] intr_mtvec,
  input                     mret,
  output  [DATA_WIDTH - 1 : 0] mret_mepc
);
  reg [DATA_WIDTH-1:0] mepc;
  reg [DATA_WIDTH-1:0] mstatus;
  reg [DATA_WIDTH-1:0] mtvec;
  reg [DATA_WIDTH-1:0] mcause;
  reg [DATA_WIDTH-1:0] mcycle;
  reg [DATA_WIDTH-1:0] mcycleh;
  reg [DATA_WIDTH-1:0] mvendorid;
  reg [DATA_WIDTH-1:0] marchid;
  

assign intr_mtvec = mtvec;
assign mret_mepc = mepc;
always @(*) begin
	case(addr)
		32'h305: csr_rdata = mtvec;
		32'h341: csr_rdata = mepc;
		32'h342: csr_rdata = mcause;
		32'h300: csr_rdata = mstatus;
		32'hb00: csr_rdata = mcycle;
		32'hb01: csr_rdata = mcycleh;
		32'hf11: csr_rdata = mvendorid;
		32'hf12: csr_rdata = marchid;
		default: ;
	endcase
end
  



always @(posedge clk or posedge rst) begin
	if(rst) begin
		mcycle <= 32'h0;
		mcycleh <= 32'h0;
	end
	else begin
		if(mcycle == 32'hffffffff) begin
			mcycle <= 32'h0;
			mcycleh <= mcycleh + 1;
		end
		else begin
			mcycle <= mcycle + 1;
		end
	end
end


  always @(posedge clk or posedge rst) begin
    if (rst) begin
		
      mepc <= {DATA_WIDTH{1'b0}};
	  mstatus <= 32'h1800;
	  mtvec <= {DATA_WIDTH{1'b0}};
	  mcause <= {DATA_WIDTH{1'b0}};
	  mvendorid <= 32'h79737978;
	  marchid <= 32'h17DC68F;
    end
    else if (wen && addr != 32'b0) begin	
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
	end 
	else if (mret) begin
	end	
  end

  always @(*) begin
	set_csr(mepc, mtvec, mcause, mstatus, mcycle, mcycleh, mvendorid, marchid);
  end
endmodule


  