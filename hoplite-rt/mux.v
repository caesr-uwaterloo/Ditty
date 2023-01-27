`timescale 1ps / 1ps

module mux
#(
	parameter integer P_W = 32
) 
(
	input  wire [P_W-1:0] in0,		//W
	input  wire [P_W-1:0] in1,		//N
	input  wire [P_W-1:0] in2,		//PE
	input  wire [1:0]     sel,
	output wire [P_W-1:0] outx,		//E
	output wire [P_W-1:0] outy		//S
);

	assign outx = (sel == 2'b10) ? in2 : (sel == 2'b01) ? in1 : in0;
	assign outy = (sel == 2'b11) ? in2 : (sel == 2'b01) ? in0 : in1;

endmodule

