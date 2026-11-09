module decoder2x4(a,b,d0,d1,d2,d3);

input a,b;
output d0,d1,d2,d3;

wire na, nb;

not (na,a);
not (nb,b);

and (d0,na,nb);
and (d1,na,b);
and (d2,a,nb);
and (d3,a,b);

endmodule
