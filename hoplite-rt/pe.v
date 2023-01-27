`timescale 1ps / 1ps
`include "/opt/hoplites/rtl/include.h"

module pe
#(
	parameter integer P_W 	= 16,			//Data width
	parameter integer X_AW  = 2,			//X addr width of torus
	parameter integer Y_AW  = 2,			//Y addr width of torus
	parameter integer X_POS = 0,			//X position of PE
	parameter integer Y_POS = 0 			//Y position of PE
)
(
	input  wire 		  clk,
	input  wire 		  rst,
	input  wire [P_W-1:0] in_pkt,	    //input packet (from South port of switch)
	input  wire 		  in_vld,		//input packet valid
	input  wire 		  sw_rdy,		//switch received packet, sw_rcvd
	output wire [P_W-1:0] out_pkt,	    //output packet
	output wire 		  out_vld		//output packet valid
);

    // //need to parametrize
    // localparam integer       MEM_D = 10;
    // localparam [`A_W-1:0]    MYPOS = {X_POS[X_AW-1:0], Y_POS[Y_AW-1:0]};     //4 bits

	// reg [`A_W:0]             pe_mem[0:MEM_D-1];     //for {valid, addr}
    // reg [$clog2(MEM_D)-1:0]  mem_addr = 0;
    // reg [$clog2(MEM_D)-1:0]  pkt_cntr = 0;          //4 bits

    // initial begin
    //     $readmemb($sformatf("mem/pe_in_%0d_%0d.mem", X_POS, Y_POS), pe_mem);
    // end

    // reg [P_W-1:0] out_pkt_reg = 0;
    // reg out_vld_reg = 0;

    // parameter IDLE = 0, STREAM_DATA = 1;
    // reg state = IDLE;
    // reg tx_done = 0;
    // always @(posedge clk) begin
    //     if (rst) begin
    //         state <= IDLE;
    //     end else begin
    //         if (tx_done) begin
    //             state <= IDLE;
    //         end else begin
    //             state <= STREAM_DATA;
    //         end
    //     end
    // end

    // wire valid = (state == STREAM_DATA) && pe_mem[mem_addr][`A_W];
    // wire tran_done = sw_rdy & out_vld_reg;

    // //mem_addr management
	// always @(posedge clk) begin
	// 	if (rst) begin
    //         mem_addr         <= 0;
    //         out_vld_reg      <= 0;
    //         tx_done          <= 0;
    //         out_pkt_reg`addr <= pe_mem[mem_addr][`A_W-1:0];
	// 	end else begin
            
    //         if (state == STREAM_DATA && mem_addr < MEM_D-1) begin
    //             if (!(out_vld_reg && !sw_rdy)) begin
    //                 out_vld_reg      <= valid;
    //                 out_pkt_reg`addr <= pe_mem[mem_addr][`A_W-1:0];
    //                 mem_addr         <= mem_addr + 1;
    //             end
    //         end

    //         if (mem_addr == MEM_D-1 && tran_done) 
    //             tx_done <= 1'b1;
	// 	end
    // end    

    // //data management
    // always @(posedge clk) begin
	// 	if (rst) begin
    //         out_pkt_reg`data <= {MYPOS, {$clog2(MEM_D){1'b0}}};
	// 	end else begin
    //         if (tran_done) begin
    //             out_pkt_reg`data <= out_pkt_reg`data + 1'b1;
    //             // if (MYPOS == 0)
    //             $display("Send to PE[%0d][%0d]: %h from PE[%0d][%0d]", out_pkt_reg`addrx, out_pkt_reg`addry, out_pkt_reg`data, X_POS, Y_POS);
    //         end

    //         // if (MYPOS == 0) begin
    //         //     $display("vld=%b, rdy=%b, addr=%b, data=%d, state=%b", out_vld_reg, sw_rdy, out_pkt_reg`addr, out_pkt_reg`data, state);
    //         //     $display("memaddr=%d, memdata=%b", mem_addr, pe_mem[mem_addr]);
    //         //     $display("---");
    //         //     $display("---");
    //         // end
	// 	end
    // end

    // always @(posedge clk) begin
    //     if (in_vld) begin
    //         // if (MYPOS == 0)
    //         $display("Received by PE[%0d][%0d]: %h", X_POS, Y_POS, in_pkt`data);
    //     end
    // end

    // assign out_pkt = out_pkt_reg;
    // assign out_vld = out_vld_reg;

endmodule
