module timebase_sensor (
    input logic clk,
    input logic reset_count_echo,
    input logic reset_count_trigger,
    input logic reset_count_cycle,
    input logic en_echo,
    input logic en_trigger, 
    input logic en_cycle, 
    output logic [23:0] count_echo,
    output logic [23:0] count_trigger,
    output logic [23:0] count_cycle);

    always_ff @(posedge clk)
    begin 
        if (reset_count_echo) count_echo <= 0;
        else if (en_echo) 
        count_echo <= count_echo +1;
    end

    always_ff @(posedge clk)
    begin 
        if (reset_count_trigger) count_trigger <= 0;
        else if (en_trigger) 
        count_trigger <= count_trigger +1;
    end

    always_ff @(posedge clk)
    begin 
        if (reset_count_cycle) count_cycle <= 0;
        else if (en_cycle) 
        count_cycle <= count_cycle +1;
    end

endmodule
