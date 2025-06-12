module robot (
    input  logic clk,
    input  logic reset,
    
    // Sensor inputs
    input  logic sensor_l_in,
    input  logic sensor_m_in,
    input  logic sensor_r_in,
    
    // Motor outputs
    output logic motor_l_pwm,
    output logic motor_r_pwm,
    
    // UART interface
    input  logic uart_rx,
    output logic uart_tx,

    // Sonar sensor
    input echo,
    output trigger,

    output [4:0] state_value
);

    // Internal signals
    logic [20:0] count;
    logic sensor_l_out;
    logic sensor_m_out;
    logic sensor_r_out;
    
    // Control signals
    logic count_reset;
    logic motor_l_direction;
    logic motor_l_reset;
    logic motor_r_direction;
    logic motor_r_reset;
    
    // Crossing counter signals
    logic reset_crossing_counter;
    logic [1:0] crossing_counter;
    
    // UART signals
    logic [7:0] tx_data;
    logic       tx_valid;
    logic       tx_ready;
    logic [7:0] rx_data;
    logic       rx_valid;
    logic       rx_ready;
    
    // Mode control signals
    logic station_reached_start;
    logic turn_crossing_start;
    logic line_follower_start;
    
    // Controller outputs
    logic count_reset_controller;
    logic motor_l_reset_controller;
    logic motor_l_direction_controller;
    logic motor_r_reset_controller;
    logic motor_r_direction_controller;
    logic reset_crossing_counter_controller;
    
    // Turns crossing outputs
    logic count_reset_turns_crossing;
    logic motor_l_reset_turns_crossing;
    logic motor_l_direction_turns_crossing;
    logic motor_r_reset_turns_crossing;
    logic motor_r_direction_turns_crossing;
    logic reset_crossing_counter_turns_crossing;
    logic reset_station;
    logic brake;
    logic activate_sensor;

    // Timebase sensor outputs
    logic [23:0] count_echo;
    logic [23:0] count_trigger;
    logic [23:0] count_cycle;

    // Sonar sensor outputs
    logic reset_count_trigger;
    logic reset_count_echo;
    logic reset_count_cycle;
    logic en_trigger;
    logic en_echo;
    logic en_cycle;
    logic [7:0] check_distance;

    // Module instances
    timebase i_timebase (
        .clk(clk),
        .reset(count_reset),
        .count(count)
    );
    
    motorcontrol i_motor_l (
        .clk(clk),
        .reset(motor_l_reset),
        .direction(motor_l_direction),
        .count_in(count),
        .pwm(motor_l_pwm),
        .brake(brake)
    );
    
    motorcontrol i_motor_r (
        .clk(clk),
        .reset(motor_r_reset),
        .direction(motor_r_direction),
        .count_in(count),
        .pwm(motor_r_pwm),
        .brake(brake)
    );
    
    inputbuffer i_inputbuffer (
        .clk(clk),
        .sensor_l_in(sensor_l_in),
        .sensor_m_in(sensor_m_in),
        .sensor_r_in(sensor_r_in),
        .sensor_l_out(sensor_l_out),
        .sensor_m_out(sensor_m_out),
        .sensor_r_out(sensor_r_out)
    );
    
    controller i_controller (
        .clk(clk),
        .reset(reset),
        .sensor_l(sensor_l_out),
        .sensor_m(sensor_m_out),
        .sensor_r(sensor_r_out),
        .count_in(count),
        .crossing_counter(crossing_counter),
        .line_follower_start(line_follower_start),
        .station_reached_start(station_reached_start),
        .reset_crossing_counter(reset_crossing_counter),
        .reset_station(reset_station),
        .count_reset(count_reset_controller),
        .turn_crossing_start(turn_crossing_start),
        .motor_l_reset(motor_l_reset_controller),
        .motor_l_direction(motor_l_direction_controller),
        .motor_r_reset(motor_r_reset_controller),
        .motor_r_direction(motor_r_direction_controller)
    );
    
    crossing_counter i_crossing_counter (
        .clk(clk),
        .reset(reset),
        .reset_crossing_counter(reset_crossing_counter),
        .reset_station(reset_station),
        .sensor_l(sensor_l_out),
        .sensor_m(sensor_m_out),
        .sensor_r(sensor_r_out),
        .crossing_counter(crossing_counter)
    );
    
    uart i_uart (
        .clk(clk),
        .reset(reset),
        .rx(uart_rx),
        .tx(uart_tx),
        .tx_data(tx_data),
        .tx_valid(tx_valid),
        .tx_ready(tx_ready),
        .rx_data(rx_data),
        .rx_valid(rx_valid),
        .rx_ready(rx_ready)
    );
    
    turns_crossing i_turns_crossing (
        .clk(clk),
        .reset(reset),
        .tx_ready(tx_ready),
        .rx_valid(rx_valid),
        .rx_data(rx_data),
        .check_distance(check_distance),
        .count_in(count),
        .turn_crossing_start(turn_crossing_start),
        .station_reached_start(station_reached_start),
        .rx_ready(rx_ready),
        .tx_data(tx_data),
        .tx_valid(tx_valid),
        .count_reset(count_reset_turns_crossing),
        .line_follower_start(line_follower_start),
        .motor_l_reset(motor_l_reset_turns_crossing),
        .motor_l_direction(motor_l_direction_turns_crossing),
        .motor_r_reset(motor_r_reset_turns_crossing),
        .motor_r_direction(motor_r_direction_turns_crossing),
        .sensor_l(sensor_l_out),
        .sensor_m(sensor_m_out),
        .sensor_r(sensor_r_out),
        .brake(brake),
	    .activate_sensor(activate_sensor),
        .state_value(state_value)
    );
    
    multiplexer i_multiplexer (
        .count_reset_controller(count_reset_controller),
        .count_reset_turns_crossing(count_reset_turns_crossing),
        .motor_l_reset_controller(motor_l_reset_controller),
        .motor_l_reset_turns_crossing(motor_l_reset_turns_crossing),
        .motor_l_direction_controller(motor_l_direction_controller),
        .motor_l_direction_turns_crossing(motor_l_direction_turns_crossing),
        .motor_r_reset_controller(motor_r_reset_controller),
        .motor_r_reset_turns_crossing(motor_r_reset_turns_crossing),
        .motor_r_direction_controller(motor_r_direction_controller),
        .motor_r_direction_turns_crossing(motor_r_direction_turns_crossing),
        .turn_crossing_start(turn_crossing_start),
        .station_reached_start(station_reached_start),
        .count_reset(count_reset),
        .motor_r_reset(motor_r_reset),
        .motor_l_reset(motor_l_reset),
        .motor_l_direction(motor_l_direction),
        .motor_r_direction(motor_r_direction)
    );

    timebase_sensor i_timebase_sensor (
	    .clk(clk),
        .reset_count_echo(reset_count_echo),
        .reset_count_trigger(reset_count_trigger),
        .reset_count_cycle(reset_count_cycle),
        .en_echo(en_echo),
        .en_trigger(en_trigger),
        .en_cycle(en_cycle),
        .count_echo(count_echo),
        .count_trigger(count_trigger),
        .count_cycle(count_cycle)
    );

    sonar_sensor i_sonar_sensor (
        .clk(clk),
        .reset(reset),
        .echo(echo),
        .count_trigger(count_trigger),
        .count_echo(count_echo),
        .count_cycle(count_cycle),
        .reset_count_trigger(reset_count_trigger),
        .reset_count_echo(reset_count_echo),
        .reset_count_cycle(reset_count_cycle),
        .en_trigger(en_trigger),
        .en_echo(en_echo),
        .en_cycle(en_cycle),
        .trigger(trigger),
        .check_distance(check_distance),
	    .activate_sensor(activate_sensor)
    );

endmodule
