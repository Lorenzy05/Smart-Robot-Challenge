module multiplexer 
   (input logic count_reset_controller,
    input logic count_reset_turns_crossing,

    input logic motor_l_reset_controller,
    input logic motor_l_reset_turns_crossing,
    
    input logic motor_l_direction_controller,
    input logic motor_l_direction_turns_crossing,

    input logic motor_r_reset_controller,
    input logic motor_r_reset_turns_crossing,
    
    input logic motor_r_direction_controller,
    input logic motor_r_direction_turns_crossing,

    input logic turn_crossing_start,
    input logic station_reached_start,

    output logic count_reset,
    output logic motor_l_reset,
    output logic motor_l_direction,
    output logic motor_r_reset,
    output logic motor_r_direction
    );

    always_comb begin
        if (turn_crossing_start || station_reached_start) begin
            count_reset = count_reset_turns_crossing;
            motor_l_reset = motor_l_reset_turns_crossing;
            motor_r_reset = motor_r_reset_turns_crossing;
            motor_l_direction = motor_l_direction_turns_crossing;
            motor_r_direction = motor_r_direction_turns_crossing;

        end

        else begin 
            count_reset = count_reset_controller;
            motor_l_reset = motor_l_reset_controller;
            motor_r_reset = motor_r_reset_controller;
            motor_l_direction = motor_l_direction_controller;
            motor_r_direction = motor_r_direction_controller;
        end
    end
endmodule    