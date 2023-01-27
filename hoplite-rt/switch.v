`timescale 1ps / 1ps
`include "/opt/hoplites/rtl/include.h"

module switch
#(
	parameter integer      P_W 	 = 32,			//Data width
	parameter integer      X_AW  = 2,			//X addr width of torus
	parameter integer      Y_AW  = 2,			//Y addr width of torus
	parameter integer      X_POS = 0,			//X position of this switch
	parameter integer      Y_POS = 0			//Y position of this switch
)
(
	input  wire 		   clk,
	input  wire 		   rst,
	input  wire [P_W-1:0]  xin_pkt,			    //West pkt
	input  wire 		   xin_vld,			    //West pkt valid
	input  wire [P_W-1:0]  yin_pkt,			    //North pkt
	input  wire			   yin_vld,			    //North pkt valid
	input  wire [P_W-1:0]  pein_pkt,			//PE pkt
	input  wire 		   pein_vld,			//PE pkt valid

	output wire [P_W-1:0]  xout_pkt,			//East output
	output wire			   xout_vld,			//East outout valid
	output wire [P_W-1:0]  yout_pkt,			//South output
	output wire			   yout_vld,			//South output valid for next router
	output wire 		   peout_vld,			//South output valid for PE
	output wire			   peout_rdy			//received pkt from PE
);
	
    /* Signals declarations */
	//registered signals
    reg  [P_W-1:0]  xout_pkt_reg, yout_pkt_reg;
	reg			    xout_vld_reg, yout_vld_reg, peout_vld_reg = 0;
    reg 		    peout_rdy_reg;
    
    //arbiter signals
	wire [P_W-1:0]  sel_xout_pkt, sel_yout_pkt;		            //selected output pkt
	wire 		    sel_xout_vld, sel_yout_vld, sel_peout_vld;  //selected pkt valid
    wire            sel_peout_rdy;
	wire [1:0]	    select;				            //common select signal for two muxes
    
    //address signals
	wire [X_AW-1:0] xin_addrx, pein_addrx;
	wire [Y_AW-1:0] xin_addry, yin_addry;
    
    //arbitration decisions
	wire            xin_gets_xout,  xin_gets_yout;
	wire            yin_gets_xout,  yin_gets_yout;
	wire            pein_gets_xout, pein_gets_yout;
	
    /* Start of the logic */
	//retrieving addresses
	assign xin_addrx      = xin_pkt`addrx;
	assign xin_addry      = xin_pkt`addry;
	assign yin_addry      = yin_pkt`addry;
	assign pein_addrx     = pein_pkt`addrx;
	
    //who gets what
	assign xin_gets_xout  = (xin_vld) & (xin_addrx != X_POS);	//W goes to E (default)
	assign xin_gets_yout  = (xin_vld) & (xin_addrx == X_POS);	//W goes to S (turn)
	assign yin_gets_xout  = (yin_vld) & (xin_gets_yout);		//N goes to E (conflict, S busy by W)
	assign yin_gets_yout  = (yin_vld) & (!xin_gets_yout);		//N goes to S (default,  S free)
    assign pein_gets_xout = (pein_vld) & (pein_addrx != X_POS) & (!xin_vld);
	assign pein_gets_yout = (pein_vld) & (pein_addrx == X_POS) & (!xin_gets_yout) & (!yin_gets_yout);
	
    //arbiter/mux select logic
	assign select[1]      = (!xin_vld) | ((!yin_vld) & (xin_addrx != X_POS));
	assign select[0]      = (xin_gets_yout) | (pein_gets_yout) | ((xin_vld) & (!yin_vld));
	
    //arbiter decisions: output valids and ready
	assign sel_xout_vld   = (xin_gets_xout) | (yin_gets_xout) | (pein_gets_xout);
    assign sel_yout_vld   = ((xin_gets_yout) & (xin_addry != Y_POS)) 		//W  passing to S
					      | ((yin_gets_yout) & (yin_addry != Y_POS))		//N  passing to S
					      | (pein_gets_yout);						        //PE passing to S
	assign sel_peout_vld  = ((xin_gets_yout) & (xin_addry == Y_POS)) 		//W arrived, ->S/PE
					      | ((yin_gets_yout) & (yin_addry == Y_POS));		//N arrived, ->S/PE
	assign sel_peout_rdy  = (pein_gets_xout) | (pein_gets_yout);

	mux #(.P_W(P_W)) mux_inst
	(
		.in0 ( xin_pkt      ),
		.in1 ( yin_pkt      ),
		.in2 ( pein_pkt     ),
		.sel ( select	    ),
		.outx( sel_xout_pkt ),
		.outy( sel_yout_pkt )
	);

	always @(posedge clk) begin
		if (rst) begin
            xout_pkt_reg   <= 0;
			xout_vld_reg   <= 1'b0;
			
            yout_pkt_reg   <= 0;
			yout_vld_reg   <= 1'b0;
			
            peout_vld_reg  <= 1'b0;
			// peout_rdy_reg  <= 1'b0;
		end else begin
			xout_pkt_reg   <= sel_xout_pkt;
			xout_vld_reg   <= sel_xout_vld;
            
            yout_pkt_reg   <= sel_yout_pkt;
			yout_vld_reg   <= sel_yout_vld;

			peout_vld_reg  <= sel_peout_vld;
			// peout_rdy_reg  <= sel_peout_rdy;
		end
	end

    //assign out wires = regs
	assign xout_pkt    = xout_pkt_reg;
	assign xout_vld    = xout_vld_reg;
  
	assign yout_pkt    = yout_pkt_reg;
	assign yout_vld    = yout_vld_reg;
  
	assign peout_vld   = peout_vld_reg;
	assign peout_rdy   = sel_peout_rdy;

endmodule
