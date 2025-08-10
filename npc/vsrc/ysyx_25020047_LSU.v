module ysyx_25020047_LSU (
    input [63:0] inst_type,
    input [31:0] raddr,
    input [31:0] waddr,
    input reg [31:0] wdata,
    input read,
    input write,
    output reg [31:0] memdata
);

reg [31:0] ram_data;
always @(*) begin
	ram_data = 0;
    if(read) begin
		//$display("raddr: 0x%08x", raddr);
        ram_data = pmem_read(raddr, 1);
    end
    else ram_data = 0;
end


//store
reg [31:0] wdata1;
wire [1:0] store_offset;
assign store_offset = waddr[1:0];
reg [31:0] sb_wmask;
always @(*) begin
    case(store_offset)
        2'b00: begin
            wdata1 = {24'b0, wdata[7:0]};
            sb_wmask = 32'h1;
        end
        2'b01: begin
            wdata1 = {16'b0, wdata[7:0], 8'b0};
            sb_wmask = 32'h2;
        end
        2'b10: begin
            wdata1 = {8'b0, wdata[7:0], 16'b0};
            sb_wmask = 32'h4;
        end
        2'b11: begin
            wdata1 = {wdata[7:0], 24'b0};
            sb_wmask = 32'h8;
        end
        default: begin
            wdata1 = wdata;
            sb_wmask = 32'h0;
        end
    endcase
end

reg [31:0] wdata2;
reg [31:0] sh_wmask;
always @(*) begin
  case (store_offset)
    2'b00: begin
      wdata2 = {16'b0, wdata[15:0]};  // halfword at byte 0
      sh_wmask = 32'h3;
    end
    2'b10: begin
      wdata2 = {wdata[15:0], 16'b0};  // halfword at byte 2
      sh_wmask  = 32'h12;
    end
    default: begin
      wdata2 = 32'b0;
      sh_wmask  = 32'h0;  // invalid address, do nothing
    end
  endcase
end



always @(*) begin
    if(write) begin
        if(inst_type == 64'h80) begin     //sw
			//$display("sw inst 0x%08x waddr 0x%08x wdata1 0x%08x wmask 0x%08x", inst, waddr, wdata1, wmask);
			pmem_write(waddr, wdata, 32'hF); //pc inst
		end
        else if(inst_type == 64'h100) begin   //sb
            //$display("sb inst 0x%08x waddr 0x%08x wdata1 0x%08x wmask 0x%08x", inst, waddr, wdata1, wmask);
            pmem_write(waddr, wdata1, sb_wmask);
        end
		else if(inst_type == 64'h200000) begin //sh
			pmem_write(waddr, wdata2, sh_wmask);
    	end
	end
end



//load
wire [1:0] load_offset;
assign load_offset = raddr[1:0];
always @(*) begin
	memdata = 32'b0;
    case(inst_type)
        64'h20: memdata = ram_data;  //lw
        64'h40: begin //lbu
			//$display("load_offset = 0x%08x", load_offset);
            //memdata = ram_data;
            case(load_offset)
                2'b00: memdata = {24'b0, ram_data[7:0]};
                2'b01: memdata = {24'b0, ram_data[15:8]};
                2'b10: memdata = {24'b0, ram_data[23:16]};
                2'b11: memdata = {24'b0, ram_data[31:24]};
				default: memdata = 32'b0;
            endcase
        end
		64'h2000000000: begin //lb
				case(load_offset)
                2'b00: memdata = {{24{ram_data[7]}}, ram_data[7:0]};
                2'b01: memdata = {{24{ram_data[15]}}, ram_data[15:8]};
                2'b10: memdata = {{24{ram_data[23]}}, ram_data[23:16]};
                2'b11: memdata = {{24{ram_data[31]}}, ram_data[31:24]};
				default: memdata = 32'b0;
            endcase
		end
		64'h4000000000: begin //lh
				case(load_offset)
                2'b00: memdata = {{16{ram_data[15]}}, ram_data[15:0]};
                2'b10: memdata = {{16{ram_data[31]}}, ram_data[31:16]};
				default: memdata = 32'b0;
            endcase
		end
		64'h8000000000: begin //lhu
				case(load_offset)
                2'b00: memdata = {16'b0, ram_data[15:0]};
                2'b10: memdata = {16'b0, ram_data[31:16]};
				default: memdata = 32'b0;
            endcase
		end
        default     : memdata = 32'b0;
    endcase
end

endmodule