module ysyx_25020047_LSU (
    input [8:0] inst_type,
    input [31:0] raddr,
    input [31:0] waddr,
    input [31:0] wdata,
    input read,
    input write,
    output reg [31:0] memdata
);

reg [31:0] ram_data;
always @(*) begin
    if(read) ram_data = pmem_read(raddr);
    else ram_data = 0;
    if(write) pmem_write(waddr, wdata, {32'b1});
    else ;
end


wire [1:0] offset;
assign offset = raddr[1:0];
always @(*) begin
    case(inst_type)
        9'b000100000: memdata = ram_data;
        9'b001000000: begin //lbu
            //memdata = ram_data;
            case(offset)
                2'b00: memdata = {24'b0, ram_data[7:0]};
                2'b01: memdata = {24'b0, ram_data[15:8]};
                2'b10: memdata = {24'b0, ram_data[23:16]};
                2'b11: memdata = {24'b0, ram_data[31:24]};
            endcase
        end
        default     : memdata = 32'b0;
    endcase
end

endmodule