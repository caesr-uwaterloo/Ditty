`timescale 1ps / 1ps
`include "/opt/hoplites/rtl/include.h"

module torus
#(
	parameter integer D_W 	    = 32,		        //Data width
	parameter integer X_DIM     = 1,		        //X dimension of torus
	parameter integer Y_DIM     = 1,		        //Y dimension of torus
	parameter integer MAX_RATE  = 1,      //1/injection rate (>= 1)
	parameter integer MAX_TOKEN = 1 			//max burst of consecutive pkts (>=1)
)
(
	input  wire             ap_clk,
	input  wire             ap_rst_n
	// AXI4-Stream (master) interface axis_mst
  //  output wire             axis_m_tvalid,
  //  input  wire             axis_m_tready,
  //  output wire [63:0]      axis_m_tdata,
  //  // AXI4-Stream (slave) interface axis_slv
  //  input  wire             axis_s_tvalid,
  //  output wire             axis_s_tready,
  //  input  wire [63:0]      axis_s_tdata
);

    localparam X_AW = (X_DIM == 1) ? 1 : $clog2(X_DIM);			//X address width
	localparam Y_AW = (Y_DIM == 1) ? 1 : $clog2(Y_DIM);			//Y address width
    localparam P_W 	= D_W + X_AW + Y_AW;		//pkt width

	wire [P_W-1:0] x_pkt [X_DIM-1:0][Y_DIM-1:0];
	wire  		   x_vld [X_DIM-1:0][Y_DIM-1:0];
	
	wire [P_W-1:0] y_pkt [X_DIM-1:0][Y_DIM-1:0];
	wire  		   y_vld [X_DIM-1:0][Y_DIM-1:0];

	wire [P_W-1:0] pe_pkt[X_DIM-1:0][Y_DIM-1:0];
	wire  		   pe_vld[X_DIM-1:0][Y_DIM-1:0];
	
	wire  		   sw_vld[X_DIM-1:0][Y_DIM-1:0];
	wire  		   sw_rdy[X_DIM-1:0][Y_DIM-1:0];
	
	reg areset = 1'b0;
	
	always @(posedge ap_clk) begin
	   areset <= ~ap_rst_n;
	end

	genvar x, y;
	generate
	for(x=0; x<X_DIM; x=x+1) begin: row
		for(y=0; y<Y_DIM; y=y+1) begin: col
			pewrap
			#(
				.P_W		( P_W 			  ),
				.X_AW		( X_AW			  ),
				.Y_AW		( Y_AW			  ),
				.X_POS		( x 			  ),
				.Y_POS		( y 			  ),
				.MAX_RATE	( MAX_RATE		  ),
				.MAX_TOKEN	( MAX_TOKEN		  )
			)
			pw
			(
				.clk		( ap_clk		  ),
				.rst		( areset 	      ),
				.in_pkt	    ( y_pkt [x][y]    ),
				.in_vld	    ( sw_vld[x][y]    ),
				.sw_rdy	    ( sw_rdy[x][y]    ),
				.out_pkt	( pe_pkt[x][y]    ),
				.out_vld	( pe_vld[x][y]    )
			);

			switch
			#(
				.P_W		( P_W   		  ),
				.X_AW		( X_AW			  ),
				.Y_AW		( Y_AW			  ),
				.X_POS		( x				  ),
				.Y_POS		( y				  )
			)
			sw
			(
				.clk		( ap_clk	      ),
				.rst		( areset		  ),
				
				.xin_pkt	( (x==0) ? x_pkt[X_DIM-1][y] : x_pkt[x-1][y] ),
				.xin_vld	( (x==0) ? x_vld[X_DIM-1][y] : x_vld[x-1][y] ),
				.yin_pkt	( (y==0) ? y_pkt[x][Y_DIM-1] : y_pkt[x][y-1] ),
				.yin_vld	( (y==0) ? y_vld[x][Y_DIM-1] : y_vld[x][y-1] ),
				.pein_pkt   ( pe_pkt[x][y]    ),
				.pein_vld   ( pe_vld[x][y]    ),

				.xout_pkt   ( x_pkt [x][y]    ),		
				.xout_vld   ( x_vld [x][y]    ),	
				.yout_pkt   ( y_pkt [x][y]    ),	
				.yout_vld   ( y_vld [x][y]    ),
				.peout_vld  ( sw_vld[x][y]    ),
				.peout_rdy  ( sw_rdy[x][y]    )
			);
		end
	end
	endgenerate

endmodule
