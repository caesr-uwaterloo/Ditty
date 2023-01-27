`timescale 1ps / 1ps

module counter
#(
	parameter integer MAX_RATE  = 1,		//1/injection rate (>= 1)
	parameter integer MAX_TOKEN = 1			//max burst of consecutive packets (>=1)
)
(
	input  wire clk,
	input  wire rst,
	input  wire	ack,						//packet accepted, decrement token
	output wire token						//token value
);

	localparam R_W = $clog2(MAX_RATE);	    //Rate cntr width
	localparam T_W = $clog2(MAX_TOKEN + 1);	//Token cntr width

	reg [R_W-1:0] rate_cntr;				//overflow after 1/ro
	reg [T_W-1:0] token_cntr;				//saturate at sigma

	always@(posedge clk) begin
		if(rst) begin
			rate_cntr <= 0;
		end else begin
			if(rate_cntr == MAX_RATE-1) begin
				rate_cntr <= 0;
			end else begin
				rate_cntr <= (rate_cntr + 1);
			end
		end
	end

	// assign token = (token_cntr > 0) && !(token_cntr == 1 && rate_cntr != MAX_RATE-1 && ack); //what's wrong with this?
	assign token = (token_cntr > 0);

	always@(posedge clk) begin
		if(rst) begin
			token_cntr <= MAX_TOKEN;
		end else begin
			if(rate_cntr == MAX_RATE-1) begin
				if(!ack && (token_cntr != MAX_TOKEN)) begin
					token_cntr <= (token_cntr + 1);
				end
			end else if(ack) begin
				token_cntr <= (token_cntr - 1);
			end
		end
	end

endmodule
