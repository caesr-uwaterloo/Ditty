`define DATA_WIDTH 256
`define X_DIMENSION 4
`define Y_DIMENSION 4
`define MAX_INJECTION_RATE 16
`define MAX_TOKEN_NUMBER 5
`define A_W X_AW+Y_AW
`define data [P_W-X_AW-Y_AW-1:0]
`define addr [P_W-1:P_W-X_AW-Y_AW]
`define addrx [P_W-1:P_W-X_AW]
`define addry [P_W-X_AW-1:P_W-X_AW-Y_AW]
