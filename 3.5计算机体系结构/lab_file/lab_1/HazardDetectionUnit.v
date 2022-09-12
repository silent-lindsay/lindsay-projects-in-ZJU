`timescale 1ps/1ps

module HazardDetectionUnit(
    input clk,
    input Branch_ID, rs1use_ID, rs2use_ID,
    input[1:0] hazard_optype_ID,
    input[4:0] rd_EXE, rd_MEM, rs1_ID, rs2_ID, rs2_EXE,
    output PC_EN_IF, reg_FD_EN, reg_FD_stall, reg_FD_flush,
        reg_DE_EN, reg_DE_flush, reg_EM_EN, reg_EM_flush, reg_MW_EN,
    output forward_ctrl_ls,
    output[1:0] forward_ctrl_A, forward_ctrl_B
);
    parameter hazard_optype_Normal  = 2'b00; //????????锟斤拷??????
    parameter hazard_optype_RIUJ    = 2'b01; //??????锟斤拷??????
    parameter hazard_optype_Store   = 2'b10; //Store???????
    parameter hazard_optype_Load    = 2'b11; //Load???????

    
    reg [1:0] hazard_optype_EX, hazard_optype_ME;
    
    always @ (posedge clk) begin //hazard_optype_ME???MEM??锟斤拷?????????hazard_optype_EX???EX??锟斤拷????????
        if(reg_EM_flush)
            hazard_optype_ME <= hazard_optype_Normal;
        else
            hazard_optype_ME <= hazard_optype_EX;
        
        if(reg_DE_flush)
            hazard_optype_EX <= hazard_optype_Normal;
        else
            hazard_optype_EX <= hazard_optype_ID;   
    end
    
    //?????锟斤拷?PC??????????
    assign reg_FD_EN = 1'b1;
    assign reg_DE_EN = 1'b1;
    assign reg_EM_EN = 1'b1;    
    assign reg_MW_EN = 1'b1;
    assign reg_EM_flush = 1'b0;
    
  

    wire RR_IDEX_A, RR_IDEX_B;
    wire RR_IDME_A, RR_IDME_B;

    wire LR_IDEX_A, LR_IDEX_B;
    wire LR_IDME_A, LR_IDME_B;

    // ----------------------------------------------------------------------
    //                         RR-Data-Hazard
    // ----------------------------------------------------------------------
    assign RR_IDEX_A =      (hazard_optype_EX == hazard_optype_RIUJ)    &&
                            rs1use_ID                                   &&
                            (rs1_ID == rd_EXE)                          &&
                            (rd_EXE != 0)                               ; 

    assign RR_IDEX_B =      (hazard_optype_EX == hazard_optype_RIUJ)    &&
                            rs2use_ID                                   &&
                            (rs2_ID == rd_EXE)                          &&
                            (rd_EXE != 0)                               ;
    
    assign RR_IDME_A =      (hazard_optype_ME == hazard_optype_RIUJ)    &&
                            rs1use_ID                                   &&
                            (rs1_ID == rd_MEM)                          &&
                            (rd_MEM != 0)                               ;

    assign RR_IDME_B =      (hazard_optype_ME == hazard_optype_RIUJ)    &&
                            rs2use_ID                                   &&
                            (rs2_ID == rd_MEM)                          &&
                            (rd_MEM != 0)                               ;

    // ----------------------------------------------------------------------
    //                         LR-Data-Hazard
    // ----------------------------------------------------------------------
    assign LR_IDEX_A =  (hazard_optype_EX == hazard_optype_Load)    &&
                        rs1use_ID                                   &&
                        (hazard_optype_ID != hazard_optype_Store)    &&
                        (rs1_ID == rd_EXE)                          &&
                        (rd_EXE != 0)                               ; 

    assign LR_IDEX_B =  (hazard_optype_EX == hazard_optype_Load)    &&
                        rs2use_ID                                   &&
                        (hazard_optype_ID != hazard_optype_Store)    &&
                        (rs2_ID == rd_EXE)                          &&
                        (rd_EXE != 0)                               ;
    
    assign LR_IDME_A =  (hazard_optype_ME == hazard_optype_Load)    &&
                        rs1use_ID                                   &&
                        // (hazard_optype_ID != hazard_optype_Store)    &&
                        (rs1_ID == rd_MEM)                          &&
                        (rd_MEM != 0)                               ;

    assign LR_IDME_B =  (hazard_optype_ME == hazard_optype_Load)    &&
                        rs2use_ID                                   &&
                        // (hazard_optype_ID != hazard_optype_Store)    &&
                        (rs2_ID == rd_MEM)                          &&
                        (rd_MEM != 0)                               ;
    
    // ----------------------------------------------------------------------
    //                         LS-Data-Hazard
    // ----------------------------------------------------------------------

    assign forward_ctrl_ls =    (hazard_optype_EX == hazard_optype_Store)    &&
                                (hazard_optype_ME == hazard_optype_Load)    &&
                                (rs2_EXE == rd_MEM)                         &&
                                (rd_MEM != 0)                               ;


    assign forward_ctrl_A =     RR_IDEX_A ? 2'b01 : ( RR_IDME_A ? 2'b10 : (LR_IDME_A ? 2'b11 : 2'b00 )) ;
    assign forward_ctrl_B =     RR_IDEX_B ? 2'b01 : ( RR_IDME_B ? 2'b10 : (LR_IDME_B ? 2'b11 : 2'b00 )) ;

    assign LRStall = LR_IDEX_A | LR_IDEX_B;
    assign PC_EN_IF = ~LRStall;

    assign reg_FD_stall = LRStall;
    assign reg_DE_flush = LRStall;

    assign reg_FD_flush = Branch_ID & ~LRStall;



endmodule