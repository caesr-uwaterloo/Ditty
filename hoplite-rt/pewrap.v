`timescale 1ps / 1ps

module pewrap
#(
	parameter integer P_W 	    = 16,		//packet width
    parameter integer X_AW      = 2,		//X addr width of torus
	parameter integer Y_AW      = 2,		//Y addr width of torus
	parameter integer X_POS     = 0,		//X position of PE
	parameter integer Y_POS     = 0,		//Y position of PE
	parameter integer MAX_RATE  = 1,		//1/injection rate (>= 1)
	parameter integer MAX_TOKEN = 1			//max burst of consecutive packets (>=1)
)
(
	input  wire 		  clk,
	input  wire 		  rst,
	input  wire [P_W-1:0] in_pkt,		    //input packet (from South port of switch)
	input  wire 		  in_vld,			//input packet valid
	input  wire 		  sw_rdy,			//switch is ready to receive packet
	output wire [P_W-1:0] out_pkt,		    //output packet
	output wire 		  out_vld			//output packet valid
);

	wire   peout_vld, token;
	assign out_vld = peout_vld & token;	    //PE packet is valid and we have token

	counter
	#(
		.MAX_RATE ( MAX_RATE  ),
		.MAX_TOKEN( MAX_TOKEN )
	)
	counter_inst
	(
		.clk  	  ( clk   	  ),
		.rst  	  ( rst   	  ),
		.ack	  ( sw_rdy    ),
		.token	  ( token 	  )
	);

	pe
	#(
		.P_W  	   ( P_W	  	 ),
        .X_AW	   ( X_AW	     ),
        .Y_AW	   ( Y_AW	     ),
		.X_POS	   ( X_POS 	     ),
		.Y_POS	   ( Y_POS 	     )
	)
	pe_inst
	(
		.clk       ( clk         ),
		.rst       ( rst         ),
		.in_pkt    ( in_pkt      ),
		.in_vld    ( in_vld      ),
		.sw_rdy    ( sw_rdy      ),
		.out_pkt   ( out_pkt     ),
		.out_vld   ( peout_vld   )
	);

endmodule
