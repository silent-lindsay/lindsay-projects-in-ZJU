`timescale 1ns / 1ps

module FU_mul(
    input clk, EN,
    input[31:0] A, B,
    output[31:0] res,
    output finish
);

    reg[6:0] state;
    assign finish = state[0] == 1'b1;
    initial begin
        state = 0;
    end

    reg[31:0] A_reg, B_reg;
    wire[31:0] mulres;

    always@(posedge clk) begin
        if(EN & state == 0) begin // state == 0
            A_reg <= A;
            B_reg <= B;
            state[6] <= 1;
        end
        else state <= (state >> 1);
    end

    multiplier mul(.CLK(clk),.A(A_reg),.B(B_reg),.P(mulres));

    assign res = mulres[31:0];

endmodule