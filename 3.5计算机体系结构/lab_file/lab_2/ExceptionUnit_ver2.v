`timescale 1ns / 1ps

module ExceptionUnit(
    input clk, rst,
    input csr_rw_in,
    input[1:0] csr_wsc_mode_in,
    input csr_w_imm_mux,
    input[11:0] csr_rw_addr_in,
    input[31:0] csr_w_data_reg,
    input[4:0] csr_w_data_imm,
    output[31:0] csr_r_data_out,

    input interrupt, // controlled by switch
    input illegal_inst,
    input l_access_fault,
    input s_access_fault,
    input ecall_m,

    input mret,

    input[31:0] epc_cur,
    input[31:0] epc_next,
    output[31:0] PC_redirect,
    output redirect_mux,

    output reg_FD_flush, reg_DE_flush, reg_EM_flush, reg_MW_flush, 
    output RegWrite_cancel
);

    reg[11:0] csr_raddr = 12'h300, csr_waddr;
    reg[31:0] csr_wdata;
    reg csr_w;
    reg[1:0] csr_wsc;

    wire[31:0] mstatus;

    CSRRegs csr(.clk(clk),.rst(rst),.csr_w(csr_w),.raddr(csr_raddr),.waddr(csr_waddr),
        .wdata(csr_wdata),.rdata(csr_r_data_out),.mstatus(mstatus),.csr_wsc_mode(csr_wsc));

    //According to the diagram, design the Exception Unit
    
//    wire csr_rw = csr_wsc_mode_in == 2'b01 ;
//    wire csr_rs = csr_wsc_mode_in == 2'b10 ;
//    wire csr_rc = csr_wsc_mode_in == 2'b11 ;

    parameter mie_addr = 12'h304 ;
    parameter mip_addr = 12'h344 ;
    parameter mepc_addr = 12'h341 ;
    parameter mtvec_addr = 12'h305 ;
    parameter mcause_addr = 12'h342 ;
    parameter mstatus_addr = 12'h300 ;
    
    reg turn_on = 0 ;
    reg[1:0] write_signal = 2'b00 ;
    reg[1:0] next_signal = 2'b00 ;
    reg[31:0] exception_reg = 32'b0 ;
    reg[31:0] epc_reg = 32'b0 ;
    reg[31:0] mstatus_reg = 32'b0 ;
    
    reg flush_reg = 0 ;
    reg flush_FD_reg, flush_DE_reg, flush_EM_reg, flush_MW_reg ;
//    reg[1:0] flush_state = 2'b0 ;
    
    reg redirect_reg = 0 ;
    
        wire exception = illegal_inst | l_access_fault | s_access_fault | ecall_m ;
        wire[31:0] exception_code = illegal_inst ? {1'b0,31'd2} : l_access_fault ? {1'b0,31'd5} : s_access_fault ? {1'b0,31'd7} :  ecall_m ? {1'b0,31'd11} : 32'd0 ;
       
        always @(posedge clk) begin
            if ( flush_reg ) 
                    begin
                                flush_FD_reg <= 0 ; 
                                flush_DE_reg <= 0 ;
                                flush_EM_reg <= 0 ;
                                flush_MW_reg <= 0 ;
                                flush_reg <= 0 ;
                    end
            if ( exception )
                    begin
                        flush_reg <= 1 ;
                        flush_FD_reg <= 1 ;
                        flush_DE_reg <= 1 ;
                        flush_EM_reg <= 1 ;
                        flush_MW_reg <= 1 ;
                    end
          
            redirect_reg <= exception ;
            write_signal <= next_signal ;
        end
        
    always @* begin
    
        if ( ~clk ) begin
                if (turn_on) begin 
                    case (write_signal )
                        2'b00:begin //write reg mepc
                            csr_waddr <= mepc_addr ;
                            csr_wdata <= epc_reg ;
                        end 
                        2'b01:begin //write reg mstatus
                            csr_waddr <= mstatus_addr ;
                            csr_wdata <= mstatus_reg ;
                        end 
                        2'b10:begin //write reg mcause
                            csr_waddr <= mcause_addr ;
                            csr_wdata <= exception_reg ;
                        end     
                        2'b11:begin
                            turn_on <= 0 ;     
                        end                        
                    endcase 
                     next_signal <= write_signal+1 ;
                end                  
        end 
//        else if ( clk )  write_signal = write_signal==2'b11 ? 2'b00 : write_signal+1 ;

                csr_wsc = csr_rw_in ? csr_wsc_mode_in : 2'b00 ; //mode the regs of CSR
                csr_w = csr_rw_in | turn_on | mret; //enable the regs to write
                if ( csr_rw_in & ~turn_on ) 
                    begin
                        csr_waddr = csr_rw_addr_in ;
                        csr_raddr = csr_rw_addr_in ;
                        if (~csr_w_imm_mux)   csr_wdata = csr_w_data_reg ;
                         else    csr_wdata = {27'b0,csr_w_data_imm} ;
                   end 
               else if ( exception & ~turn_on )
                    begin
                        turn_on = 1 ;
                        csr_raddr = mtvec_addr ;
                        epc_reg = epc_cur ;
                        exception_reg = exception_code ;
                        mstatus_reg = {mstatus[31:8],mstatus[3],mstatus[6:4],1'b0,mstatus[2:0]} ;
                    end
                else if ( mret & ~turn_on )
                    begin
                        csr_raddr = mepc_addr ;                        
                        csr_waddr = mstatus_addr ;
                        csr_wdata = {mstatus[31:4],mstatus[7],mstatus[2:0]} ;
                    end 
                                    
    end
    
    assign PC_redirect = csr_r_data_out ;
    assign redirect_mux = redirect_reg | mret ;
    
    assign reg_FD_flush = flush_FD_reg | exception | mret ;
    assign reg_DE_flush = flush_DE_reg | exception | mret ;
    assign reg_EM_flush = flush_EM_reg | exception | mret ;
    assign reg_MW_flush = flush_MW_reg | exception | mret ;
    
    assign RegWrite_cancel = exception ;

endmodule