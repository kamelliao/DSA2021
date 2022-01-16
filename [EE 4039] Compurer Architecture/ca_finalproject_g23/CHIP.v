// Your code
module CHIP(clk,
            rst_n,
            // For mem_D
            mem_wen_D,
            mem_addr_D,
            mem_wdata_D,
            mem_rdata_D,
            // For mem_I
            mem_addr_I,
            mem_rdata_I);

    input         clk, rst_n ;
    // For mem_D
    output        mem_wen_D  ;
    output [31:0] mem_addr_D ;
    output [31:0] mem_wdata_D;
    input  [31:0] mem_rdata_D;
    // For mem_I
    output [31:0] mem_addr_I ;
    input  [31:0] mem_rdata_I;
    
    //---------------------------------------//
    // Do not modify this part!!!            //
    // Exception: You may change wire to reg //
    reg    [31:0] PC          ;              //
    wire   [31:0] PC_nxt      ;              //
    wire          regWrite    ;              //
    wire   [ 4:0] rs1, rs2, rd;              //
    wire   [31:0] rs1_data    ;              //
    wire   [31:0] rs2_data    ;              //
    reg    [31:0] rd_data     ;              //
    //---------------------------------------//

    // Todo: other wire/reg
    // Mul related
    wire        valid, ready, mode;
    wire [31:0] mul_out;

    // Some data
    wire [ 6:0] op;
    wire [31:0] imm;
    wire [31:0] imm_32;

    // ALU related
    wire        zero;
    wire [ 3:0] alu_ctrl;
    wire [31:0] alu_in_A, alu_in_B;
    wire [31:0] alu_out;

    // Control signals
    wire       ctrl_jal;
    wire       ctrl_jalr;
    wire       ctrl_branch;
    wire       ctrl_memread;
    wire [1:0] ctrl_memtoreg;
    wire       ctrl_memwrite;
    wire [2:0] ctrl_aluop;
    wire       ctrl_alusrc_A;
    wire       ctrl_alusrc_B;

    next_PC nxt_pc(
        .pc_i(PC),
        .imm_i(imm_32),
        .rs1_i(rs1_data),
        .brance_i(ctrl_branch),
        .zero_i(zero),
        .jal_i(ctrl_jal),
        .jalr_i(ctrl_jalr),
        .alu_ctrl_i(alu_ctrl),
        .ready_i(ready),
        .pc_nxt_o(PC_nxt)
    );

    instr_decode decode(
        .instr_i(mem_rdata_I),
        .instr_op_o(op),
        .instr_rs1_o(rs1),
        .instr_rs2_o(rs2),
        .instr_rd_o(rd),
        .instr_imm_o(imm)
    );

    control ctrl(
        .clk(clk),
        .rst_n(rst_n),
        .opcode_i(op),
        .ctrl_jal_o(ctrl_jal),
        .ctrl_jalr_o(ctrl_jalr),
        .ctrl_branch_o(ctrl_branch),
        .ctrl_memread_o(ctrl_memread),
        .ctrl_memtoreg_o(ctrl_memtoreg),
        .ctrl_memwrite_o(ctrl_memwrite),
        .ctrl_aluop_o(ctrl_aluop),
        .ctrl_alusrc_A_o(ctrl_alusrc_A),
        .ctrl_alusrc_B_o(ctrl_alusrc_B),
        .ctrl_regwrite_o(regWrite)
    );

    ALU_control aluctrl(
        .aluop_i(ctrl_aluop),
        .imm_i(imm),
        .alu_ctrl_o(alu_ctrl)
    );

    //---------------------------------------//
    // Do not modify this part!!!            //
    reg_file reg0(                           //
        .clk(clk),                           //
        .rst_n(rst_n),                       //
        .wen(regWrite),                      //
        .a1(rs1),                            //
        .a2(rs2),                            //
        .aw(rd),                             //
        .d(rd_data),                         //
        .q1(rs1_data),                       //
        .q2(rs2_data));                      //
    //---------------------------------------//

    Imm_Gen immgen(
        .imm_i(imm),
        .imm_32_o(imm_32)
    );

    ALU alu(
        .clk(clk),
        .rst_n(rst_n),
        .alu_ctrl_i(alu_ctrl),
        .in_A_i(alu_in_A),
        .in_B_i(alu_in_B),
        .out_o(alu_out),
        .zero_o(zero)
    );

    mulDiv mul(
        .clk(clk),
        .rst_n(rst_n),
        .valid(valid),
        .ready(ready),
        .mode(mode),
        .in_A(alu_in_A),
        .in_B(alu_in_B),
        .out(mul_out)
    );

    //------------------For Next PC----------------//
    assign mem_addr_I = PC;

    //------------------For Load & Store-----------//
    assign mem_addr_D = alu_out;
    assign mem_wen_D = (ctrl_memwrite) ? 1'b1 : 1'b0;

    //------------------For Store------------------//
    assign mem_wdata_D = (ctrl_memwrite) ? rs2_data : 1'b0;

    // Todo: any combinational/sequential circuit
    //------------------Update PC-------------------//
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            PC <= 32'h00010000; // Do not modify this value!!!
        end
        else begin
            PC <= PC_nxt;
        end
    end

    //------------------ALU Input--------------------//
    assign alu_in_A = (!ctrl_alusrc_A) ? rs1_data : PC;
    assign alu_in_B = (!ctrl_alusrc_B) ? rs2_data : imm_32;

    //--------------------MulDiv---------------------//
    assign valid = (alu_ctrl == 4'b0011) ? 1 : 0;
    assign mode = (imm[14:12] == 3'b000) ? 0 : 1;

    //------------------Write Data-------------------//
    always @(*) begin
        case (ctrl_memtoreg)
            2'b00: // lw
                rd_data = mem_rdata_D;
            2'b01: // jal
                rd_data = PC + 4;
            2'b10: // R-type
                if (!ready) rd_data = alu_out;
                else        rd_data = mul_out;
            default:
                rd_data = 0;
        endcase
    end

endmodule

module next_PC(
    pc_i,
    imm_i,
    rs1_i,
    brance_i,
    zero_i,
    jal_i,
    jalr_i,
    alu_ctrl_i,
    ready_i,
    pc_nxt_o
);

    input  [31:0] pc_i, imm_i, rs1_i;
    input  [ 3:0] alu_ctrl_i;
    input         brance_i, zero_i, jal_i, jalr_i, ready_i;
    output [31:0] pc_nxt_o;

    wire branch, jump;
    reg [31:0] next_PC1, next_PC2, jalrpc, mux1, pc_nxt_o;

    assign branch = brance_i & zero_i;
    assign jump = jal_i | branch;

    always @(*) begin
        next_PC1 = pc_i + 32'd4;
        next_PC2 = pc_i + imm_i;
        jalrpc = rs1_i + imm_i;
        mux1 = (jump) ? next_PC2 : next_PC1;
        if (alu_ctrl_i == 4'b0011) pc_nxt_o = (ready_i) ? next_PC1 : pc_i;
        else                       pc_nxt_o = (jalr_i) ? jalrpc : mux1;
    end

endmodule

module instr_decode(
    instr_i,
    instr_op_o,
    instr_rs1_o,
    instr_rs2_o,
    instr_rd_o,
    instr_imm_o
);

    input  [31:0] instr_i;
    output [ 6:0] instr_op_o;
    output [ 4:0] instr_rs1_o;
    output [ 4:0] instr_rs2_o;
    output [ 4:0] instr_rd_o;
    output [31:0] instr_imm_o;

    assign instr_op_o = instr_i[6:0];
    assign instr_rs1_o = instr_i[19:15];
    assign instr_rs2_o = instr_i[24:20];
    assign instr_rd_o = instr_i[11:7];
    assign instr_imm_o = instr_i[31:0];

endmodule

module reg_file(clk, rst_n, wen, a1, a2, aw, d, q1, q2);
   
    parameter BITS = 32;
    parameter word_depth = 32;
    parameter addr_width = 5; // 2^addr_width >= word_depth
    
    input clk, rst_n, wen; // wen: 0:read | 1:write
    input [BITS-1:0] d;
    input [addr_width-1:0] a1, a2, aw;

    output [BITS-1:0] q1, q2;

    reg [BITS-1:0] mem [0:word_depth-1];
    reg [BITS-1:0] mem_nxt [0:word_depth-1];

    integer i;

    assign q1 = mem[a1];
    assign q2 = mem[a2];

    always @(*) begin
        for (i=0; i<word_depth; i=i+1)
            mem_nxt[i] = (wen && (aw == i)) ? d : mem[i];
    end

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            mem[0] <= 0;
            for (i=1; i<word_depth; i=i+1) begin
                case(i)
                    32'd2: mem[i] <= 32'hbffffff0;
                    32'd3: mem[i] <= 32'h10008000;
                    default: mem[i] <= 32'h0;
                endcase
            end
        end
        else begin
            mem[0] <= 0;
            for (i=1; i<word_depth; i=i+1)
                mem[i] <= mem_nxt[i];
        end       
    end
endmodule

module control(
    clk,
    rst_n,
    opcode_i,
    ctrl_jal_o,
    ctrl_jalr_o,
    ctrl_branch_o,
    ctrl_memread_o,
    ctrl_memtoreg_o,
    ctrl_memwrite_o,
    ctrl_aluop_o,
    ctrl_alusrc_A_o,
    ctrl_alusrc_B_o,
    ctrl_regwrite_o
);
    
    input        clk;
    input        rst_n;
    input  [6:0] opcode_i;
    output       ctrl_jal_o;
    output       ctrl_jalr_o;
    output       ctrl_branch_o;
    output       ctrl_memread_o;
    output [1:0] ctrl_memtoreg_o;
    output       ctrl_memwrite_o;
    output [2:0] ctrl_aluop_o;
    output       ctrl_alusrc_A_o;
    output       ctrl_alusrc_B_o;
    output       ctrl_regwrite_o;

    assign ctrl_jal_o = opcode_i[6] & opcode_i[3];
    assign ctrl_jalr_o = (opcode_i[6] & ~opcode_i[3]) & opcode_i[2];
    assign ctrl_branch_o = opcode_i[6] & (~opcode_i[2]);
    assign ctrl_memread_o = ~opcode_i[5] & ~opcode_i[4];
    assign ctrl_memwrite_o = (~opcode_i[6] & opcode_i[5]) & ~opcode_i[4];
    assign ctrl_memtoreg_o = (ctrl_memread_o==1'b1) ? 2'b00 ://lw
                             (opcode_i[3]==1'b1) ? 2'b01 ://jal
                             2'b10;//default

    assign ctrl_alusrc_A_o = opcode_i[4] & opcode_i[2];
    assign ctrl_alusrc_B_o = ((opcode_i[6:0] != 7'b0110011) & (opcode_i[6:0] != 7'b1100011));
    assign ctrl_aluop_o = ((opcode_i[4:2]==3'b000) & (opcode_i[6]==1'b0) | (opcode_i[6:0]==7'b0010111)) ? 3'b000 : // err1: lost auipc
                           ((opcode_i[6:5]==2'b11) & (opcode_i[2]==1'b0)) ? 3'b001 :
                            (opcode_i[6:2]==5'b11001) ? 3'b010 :
                            (opcode_i[6:2]==5'b00100) ? 3'b011 :
                            (opcode_i[6:2]==5'b01100) ? 3'b100 :
                            3'b111;
    assign ctrl_regwrite_o = (opcode_i[5:2] == 4'b1000) ? 1'b0 : 1'b1; // err2

endmodule

module ALU_control(
    aluop_i,
    imm_i,
    alu_ctrl_o
);

    input      [ 2:0] aluop_i;
    input      [31:0] imm_i;
    output reg [ 3:0] alu_ctrl_o;

    wire [1:0] funct7;
    wire [2:0] funct3;

    assign funct7 = {imm_i[30], imm_i[25]};
    assign funct3 = imm_i[14:12];

    always@ (*) begin
        case(aluop_i)
            3'b000: // (add) lw/sw
                alu_ctrl_o = 4'b0000;
            3'b001: 
                begin// B-Type
                    case(funct3)
                        3'b000:// (sub) beq
                            alu_ctrl_o = 4'b0001;
                        3'b001:// bne
                            alu_ctrl_o = 4'b0110;
                        default: // nothing
                            alu_ctrl_o = 4'b1111;
                    endcase
                end
            3'b010: // (add) jalr
                alu_ctrl_o = 4'b0000;
            3'b011: // I-Type
                begin
                    case(funct3)
                        3'b000: // (add) addi
                            alu_ctrl_o = 4'b0000;
                        3'b010: // slti
                            alu_ctrl_o = 4'b0010;
                        3'b101: // srai
                            alu_ctrl_o = 4'b0100;
                        3'b001: // slli
                            alu_ctrl_o = 4'b0101;
                        default: // nothing
                            alu_ctrl_o = 4'b1111;
                    endcase
                end                
            3'b100: // R-Type
                begin
                    case(funct7)
                        2'b00: // add
                            alu_ctrl_o = 4'b0000;
                        2'b10: // sub
                            alu_ctrl_o = 4'b0001;
                        2'b01: // mul
                            alu_ctrl_o = 4'b0011;
                        default: // nothing
                            alu_ctrl_o = 4'b1111;
                    endcase
                end  
            default: // nothing
                alu_ctrl_o = 4'b1111;
        endcase
    end

endmodule


module Imm_Gen(imm_i, imm_32_o);

    input      [31:0] imm_i;
    output reg [31:0] imm_32_o;

    wire [6:0] opcode;
    assign opcode = imm_i[6:0];

    always @(*) begin
        case(opcode)
            7'b0000011: // I-type lw
                imm_32_o = {{20{imm_i[31]}} , imm_i[31:20]};
            7'b0010011: 
                begin   //(&I-type srai &I-type slli)
                    if ((imm_i[14:12] == 3'b001)|(imm_i[14:12] == 3'b101))
                        imm_32_o = {27'b0, imm_i[24:20]};
                    else    // I-type addi &I-type slti
                        imm_32_o = {{20{imm_i[31]}} , imm_i[31:20]};
                end
            7'b0100011: // S-type sw
                imm_32_o = {{20{imm_i[31]}} , imm_i[31:25] , imm_i[11:7]};
            7'b1100011: // SB-type beq
                imm_32_o = {{20{imm_i[31]}} , imm_i[7] , imm_i[30:25] , imm_i[11:8] , 1'b0};
            7'b0010111: // U-type auipc
                imm_32_o = {imm_i[31:12] , 12'b0};
            7'b1101111: // J-type jal
                imm_32_o = {{12{imm_i[31]}}, imm_i[19:12], imm_i[20], imm_i[30:21], 1'b0}; // err2
            7'b1100111: // I-type jalr
                imm_32_o = {{20{imm_i[31]}}, imm_i[31:20]}; // err2
    //--------------R-type no imm--------------//
            /*//R-type add

            //R-type sub

            //R-type mul*/

            default: imm_32_o = 32'b0;
    //----------------------------------------//
        endcase
    end

endmodule

module ALU(
    clk,
    rst_n,
    alu_ctrl_i,
    in_A_i,
    in_B_i,
    out_o,
    zero_o
);

    input             clk, rst_n;
    input      [ 3:0] alu_ctrl_i;
    input      [31:0] in_A_i, in_B_i;
    output reg [31:0] out_o;
    output reg        zero_o;

    always @(*) begin
        if (alu_ctrl_i == 4'b0110) zero_o = (in_A_i != in_B_i) ? 1: 0;  // BNE
        else                       zero_o = (in_A_i == in_B_i) ? 1: 0;  // BEQ

        case (alu_ctrl_i)
            4'b0000: // ADD
                out_o = in_A_i + in_B_i;
            4'b0001: // SUB
                out_o = in_A_i - in_B_i;
            4'b0010: // SLTI
                out_o = (in_A_i < in_B_i) ? 1 : 0;
            4'b0100: // SRAI
                out_o = in_A_i >>> in_B_i;
            4'b0101: // SLLI
                out_o = in_A_i << in_B_i;
            default:
                out_o = 32'b0;
        endcase
    end
endmodule


module mulDiv(
    clk,
    rst_n,
    valid,
    ready,
    mode,
    in_A,
    in_B,
    out
);

    // Definition of ports
    input         clk, rst_n;
    input         valid, mode; // mode: 0: mulu, 1: divu
    output        ready;
    input  [31:0] in_A, in_B;
    output [31:0] out;

    // Definition of states
    parameter IDLE = 2'b00;
    parameter MUL  = 2'b01;
    parameter DIV  = 2'b10;
    parameter OUT  = 2'b11;

    // Todo: Wire and reg if needed
    reg  [ 1:0] state = IDLE, state_nxt = IDLE;
    reg  [ 4:0] counter = 5'b0, counter_nxt = 5'b0;
    reg  [63:0] shreg = 64'b0, shreg_nxt = 64'b0;
    reg  [31:0] alu_in = 32'b0, alu_in_nxt = 32'b0;
    reg  [32:0] alu_out = 32'b0;

    // Todo: Instatiate any primitives if needed

    // Todo 5: Wire assignments
    assign out = shreg[31:0];
    assign ready = (state == OUT) ? 1'b1 : 1'b0;

    // Combinational always block
    // Todo 1: Next-state logic of state machine
    always @(*) begin
        case(state)
            IDLE: begin
                if (valid) begin
                    if (mode) state_nxt = DIV;
                    else      state_nxt = MUL;
                end
                else state_nxt = IDLE;
            end
            MUL : begin
                if (counter == 31) state_nxt = OUT;
                else               state_nxt = MUL;
            end
            DIV : begin
                if (counter == 31) state_nxt = OUT;
                else               state_nxt = DIV;
            end
            OUT : state_nxt = IDLE;
        endcase
    end

    // Todo 2: Counter
    always @(*) begin
        if (state == MUL || state == DIV) counter_nxt = counter + 1;
        else                              counter_nxt = 5'b0;
    end
    
    // ALU input
    always @(*) begin
        case(state)
            IDLE: begin
                if (valid) alu_in_nxt = in_B;
                else       alu_in_nxt = 32'b0;
            end
            OUT : alu_in_nxt = 32'b0;
            default: alu_in_nxt = alu_in;
        endcase
    end

    // Todo 3: ALU output
    always @(*) begin
        case(state)
            MUL: begin
                if (shreg[0]) alu_out = {1'b0, shreg[63:32]} + {1'b0, alu_in};
                else          alu_out = {1'b0, shreg[63:32]};
            end
            DIV:              alu_out = {1'b0, shreg[62:31]} - {1'b0, alu_in};
            default:          alu_out = 33'b0;
        endcase
    end

    // Todo 4: Shift register
    always @(*) begin
        case(state)
            IDLE: begin
                if (valid) shreg_nxt = {32'b0, in_A};
                else       shreg_nxt = 64'b0;
            end
            MUL: shreg_nxt = {alu_out[32:0], shreg[31:1]};
            DIV: begin
                if (alu_out > {1'b0, shreg[62:31]}) shreg_nxt = {shreg[62:0], 1'b0};
                else                                shreg_nxt = {alu_out[31:0], shreg[30:0], 1'b1};
                end 
            default: shreg_nxt = shreg;
        endcase
    end

    // Todo: Sequential always block
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            state <= IDLE;
            counter <= 0;
            alu_in <= 0;
            shreg <= 0;
        end
        else begin
            state <= state_nxt;
            counter <= counter_nxt;
            alu_in <= alu_in_nxt;
            shreg <= shreg_nxt;
        end
    end

endmodule
