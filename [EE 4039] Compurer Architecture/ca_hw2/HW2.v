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
    output [63:0] out;

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
    assign out = shreg;
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
        end
        else begin
            state <= state_nxt;
            counter <= counter_nxt;
            alu_in <= alu_in_nxt;
            shreg <= shreg_nxt;
        end
    end

endmodule