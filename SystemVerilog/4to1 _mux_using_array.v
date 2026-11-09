module mux4to1(
    input  [3:0] data,
    input  [1:0] sel,
    output mux_out
);

assign mux_out = data[sel];

endmodule
