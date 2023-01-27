module hoplite 
#(
	parameter integer P_W  =32,		        //Data width
	parameter integer X_DIM = 1,		        //X dimension of torus
	parameter integer Y_DIM = 1,		        //Y dimension of torus
	parameter integer MAX_RATE = 1,		        //Y dimension of torus
	parameter integer MAX_TOKEN	= 1	        //Y dimension of torus
)
(
  input wire clk,
  input wire rst,

  // ideally, we don't want to use packed array
  output [X_DIM*Y_DIM*P_W -1:0]  to_ext_data_o ,
  output [X_DIM*Y_DIM-1:0]       to_ext_valid_o,

  input  [X_DIM*Y_DIM*P_W - 1:0] from_ext_data_i ,
  input  [X_DIM*Y_DIM-1:0]       from_ext_valid_i,
  output [X_DIM*Y_DIM-1:0]       from_ext_ready_o
);

  torus #(
    .D_W(P_W - ( (X_DIM == 1) ? 1 : $clog2(X_DIM)) - ( (Y_DIM == 1) ? 1 : $clog2(Y_DIM))),
    .X_DIM(X_DIM),
    .Y_DIM(Y_DIM),
    .MAX_RATE(MAX_RATE),
    .MAX_TOKEN(MAX_TOKEN)
  )
  torus_inst (
    .ap_clk(clk),
    .ap_rst_n(!rst)
  );

  wire[P_W-1:0] to_ext_data [0:X_DIM*Y_DIM-1];
  wire          to_ext_valid[0:X_DIM*Y_DIM-1];

  wire[P_W-1:0] from_ext_data [0:X_DIM*Y_DIM-1];
  wire          from_ext_valid[0:X_DIM*Y_DIM-1];
  wire          from_ext_ready[0:X_DIM*Y_DIM-1];

  genvar i, j;
  generate

  for(i = 0; i < X_DIM; i = i + 1) begin
    for(j = 0; j < Y_DIM; j = j + 1) begin
      assign to_ext_data_o [i * Y_DIM * P_W + j * P_W +: P_W] = to_ext_data[i * Y_DIM + j];
      assign to_ext_valid_o[i * Y_DIM + j]        = to_ext_valid[i * Y_DIM + j];

      assign from_ext_data[i * Y_DIM + j]    = from_ext_data_i[i * Y_DIM * P_W + j * P_W  +: P_W];
      assign from_ext_valid[i * Y_DIM + j]   = from_ext_valid_i[i * Y_DIM + j];
      assign from_ext_ready_o[i * Y_DIM + j] = from_ext_ready[i * Y_DIM + j];

      assign to_ext_data[i * Y_DIM + j] = torus_inst.row[i].col[j].pw.pe_inst.in_pkt;
      assign to_ext_valid[i * Y_DIM + j] = torus_inst.row[i].col[j].pw.pe_inst.in_vld;

      assign from_ext_ready[i * Y_DIM + j] = torus_inst.row[i].col[j].pw.pe_inst.sw_rdy;
      assign torus_inst.row[i].col[j].pw.pe_inst.out_pkt = from_ext_data[i * Y_DIM + j];
      assign torus_inst.row[i].col[j].pw.pe_inst.out_vld = from_ext_valid[i * Y_DIM + j];
    end
  end
  endgenerate



endmodule
