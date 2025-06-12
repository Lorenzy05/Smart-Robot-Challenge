module controller
   (input logic clk,
    input logic reset,

    input logic sensor_l,
    input logic sensor_m,
    input logic sensor_r,

    input logic [20:0] count_in,
    input logic [1:0] crossing_counter,
    input logic line_follower_start,
    
    output logic count_reset,
    output logic turn_crossing_start,
	output logic station_reached_start,

	output logic reset_station,
	output logic reset_crossing_counter,

    output logic motor_l_reset,
    output logic motor_l_direction,

    output logic motor_r_reset,
    output logic motor_r_direction
    );

	typedef enum logic [3:0] {central, forward, gentle_left, sharp_left, gentle_right, sharp_right, crossing_decission, forward_central, forward_crossing, turn_crossing_activate, station_reached_activate} statetype;
	statetype state, nextstate;

	logic [4:0] turn_counter_forward;
	logic start;

	always_ff @(posedge clk) begin
		if (reset) begin
			state <= central;
			turn_counter_forward <= 0;
			start <= 1;
		end
		else begin
            state <= nextstate;

		if (state == forward_central)
            turn_counter_forward <= turn_counter_forward + 1;
		else if (state == central)
			turn_counter_forward <= 0;
		else if (sensor_m == 0)
		    start <= 0;
		end
    end

	always_comb begin 

  		motor_l_reset = 1;
  		motor_r_reset = 1;
  		motor_l_direction = 0;
  		motor_r_direction = 0;
  		count_reset = 1;
        turn_crossing_start = 0;
		station_reached_start = 0;
		reset_station = 0;
		reset_crossing_counter = 0;
  		nextstate = state;


		case(state)
		central: begin //central state
			
			if (sensor_l == 0 && sensor_m == 0 && sensor_r == 0)
				nextstate = forward_central;
			else if (sensor_l == 0 && sensor_m == 1 && sensor_r == 0)
				nextstate = forward;
			else if (sensor_l == 1 && sensor_m == 0 && sensor_r == 1)
				nextstate = forward;
			else if (sensor_l == 0 && sensor_m == 0 && sensor_r == 1)
				nextstate = gentle_left;
			else if (sensor_l == 0 && sensor_m == 1 && sensor_r == 1)
				nextstate = sharp_left;
			else if (sensor_l == 1 && sensor_m == 0 && sensor_r == 0)
				nextstate = gentle_right;
			else if (sensor_l == 1 && sensor_m == 1 && sensor_r == 0)
				nextstate = sharp_right;
			else if (sensor_l == 1 && sensor_m == 1 && sensor_r == 1 && start == 0) begin
				nextstate = station_reached_activate;
				station_reached_start = 1;
			end
			else if (sensor_l == 1 && sensor_m == 1 && sensor_r == 1 && start == 1)
			    nextstate = forward;
			else
				nextstate = central;
		end

		forward: begin //forward
			motor_l_reset = 0;
			motor_r_reset = 0;
			motor_l_direction = 1;
			motor_r_direction = 0;
			count_reset = 0;

			if (count_in >= 21'd2_000_000)
				nextstate = central;

		end

		gentle_left: begin //gentle left
			motor_l_reset = 1;
			motor_r_reset = 0;
			motor_l_direction = 0;
			motor_r_direction = 0;
			count_reset = 0;

			if (count_in >= 21'd2_000_000)
				nextstate = central;

		end

		sharp_left: begin //sharp left
			motor_l_reset = 0;
			motor_r_reset = 0;
			motor_l_direction = 0;
			motor_r_direction = 0;
			count_reset = 0;

			if (count_in >= 21'd2_000_000)
				nextstate = central;

		end

		gentle_right: begin //gentle right
			motor_l_reset = 0;
			motor_r_reset = 1;
			motor_l_direction = 1;
			motor_r_direction = 0;
			count_reset = 0;

			if (count_in >= 21'd2_000_000)
				nextstate = central;

		end

		sharp_right: begin //sharp right
			motor_l_reset = 0;
			motor_r_reset = 0;
			motor_l_direction = 1;
			motor_r_direction = 1;
			count_reset = 0;

			if (count_in >= 21'd2_000_000)
				nextstate = central;

		end

		forward_central: begin //this state makes the robot pass the crossing
			if (turn_counter_forward < 11)
				nextstate = forward_crossing;
			else
				nextstate = crossing_decission;

		end

		forward_crossing: begin //goes forward and returns to forward_central
			motor_l_reset = 0;
			motor_r_reset = 0;
			motor_l_direction = 1;
			motor_r_direction = 0;
			count_reset = 0;

			if (count_in >= 21'd2_000_000)
				nextstate = forward_central;

		end	

		crossing_decission: begin //decides wheter there is a crossing or not

			if (crossing_counter < 2)
				nextstate = central;
			else
				nextstate = turn_crossing_activate;

		end

        turn_crossing_activate: begin //activates the module turns_crossing
			turn_crossing_start = 1;
			reset_crossing_counter = 1;

            if (line_follower_start) begin
                turn_crossing_start = 0;
				reset_crossing_counter = 1;
                nextstate = central;
			end
        
		end

		station_reached_activate: begin //activates the module turns_crossing
			station_reached_start = 1;
			reset_station = 1;

            if (line_follower_start) begin
                station_reached_start = 0;
				reset_station = 1;
                nextstate = central;
			end
        
		end
		
		default: begin
            motor_l_reset = 1;
            motor_r_reset = 1;
            motor_l_direction = 0;
            motor_r_direction = 0;
            count_reset = 1;
            turn_crossing_start = 0;
            station_reached_start = 0;
            reset_station = 0;
            reset_crossing_counter = 0;
            nextstate = state;
		end
			
		
		endcase
	end

endmodule
   
