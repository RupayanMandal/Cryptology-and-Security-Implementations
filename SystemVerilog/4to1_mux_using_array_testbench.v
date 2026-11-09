module tb;

reg  [3:0] data;
reg  [1:0] sel;
wire out;

mux4to1 dut(data, sel, out);

integer i;

initial begin

    data = 4'b1010;

    for(i=0;i<4;i=i+1)
    begin
        sel = i;
        #5;
    end

end

endmodule
