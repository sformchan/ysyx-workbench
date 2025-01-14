module top(
	input [1:0] sw,
	output [15:0] ledr
);
	assign ledr = {16{sw[1] ^ sw[0]}};
endmodule
