module turns_crossing
   (input logic clk,
    input logic reset,

    input logic tx_ready,
    input logic rx_valid,
    input logic [7:0] rx_data,
	input logic [7:0] check_distance,

    input logic [20:0] count_in,
    input logic turn_crossing_start,
	input logic station_reached_start,

	input logic sensor_l,
    input logic sensor_m,
    input logic sensor_r,
    
	output logic count_reset,
	output logic brake,

	output logic line_follower_start,
	output logic activate_sensor,

    output logic motor_l_reset,
    output logic motor_l_direction,

    output logic motor_r_reset,
    output logic motor_r_direction,

    output logic rx_ready,
    output logic [7:0] tx_data,
    output logic tx_valid,

	output logic [4:0] state_value

	output logic timeout_crossing_fix, // 新增

    );

	typedef enum logic [4:0] {IDLE, brake_state, off_line_reset, brake_state_2, sending_message, sending_message_2, sending_message_sensor, sending_message_sensor_2, received_message, decission_direction, right, left, turnaround, turnaround_central, left_central, right_central, line_follower_activate, backward_central, backward} statetype;
	statetype state, nextstate;

	logic off_line;
	localparam TIMEOUT = 500_000_000;
	logic [31:0] timeout_count;

	always_ff @(posedge clk) begin
		if (reset) begin
			state <= IDLE;
			off_line <= 0;
			timeout_count <=0;
		end
		else begin
            state <= nextstate;

		if (sensor_l == 1 && sensor_m == 1 && sensor_r == 1) //make the condition for off_line more strict
			off_line <= 1;
		else if (state == decission_direction || state == off_line_reset)
			off_line <= 0;
        if(state==sending_message || state == sending_message_sensor)begin
            if(timeout_count < TIMEOUT)
                timeout_count <= timeout_count + 1;
        end
        else begin
            timeout_count <= 0;
        end
        end
    end

	always_comb begin 

  		rx_ready = 0;
  		tx_valid = 0;
  		tx_data = 8'h00;
  		motor_l_reset = 1;
  		motor_r_reset = 1;
  		motor_l_direction = 0;
  		motor_r_direction = 0;
  		count_reset = 1;
        line_follower_start = 0;
		brake = 0;
		activate_sensor = 0;
  		nextstate = state;
		state_value = 5'b00000;

		case(state)
        IDLE: begin //Idle state, line follower is active
			state_value = 5'b00001;

            if (turn_crossing_start || station_reached_start)
                nextstate = brake_state;
        end

		brake_state: begin //when brake equals 1 the PWM signal is 1.5ms, which causes the robot to brake
			state_value = 5'b00010;
			motor_l_reset = 0;
			motor_r_reset = 0;
			brake = 1;
			activate_sensor = 1;
			count_reset = 0;

			if (count_in >= 21'd2_000_000 && turn_crossing_start == 1)
				nextstate = sending_message;
			else if (count_in >= 21'd2_000_000 && station_reached_start == 1)
				nextstate = right;
		end

		sending_message: begin //sends a + to the PC
			state_value = 5'b00011;
			tx_data = 8'b00101011;
            if (tx_ready) begin
				nextstate = sending_message_sensor;
				tx_valid = 1;
				timeout_crossing_fix = 0; // 新增
			end else if(timeout_count >= TIMEOUT) begin
			    nextstate = received_message;
				timeout_crossing_fix = 1; // 新增
			end else begin
				tx_valid = 0;
				nextstate = sending_message;
				timeout_crossing_fix = 0; // 新增
			end
		end

		sending_message_sensor: begin //sends the distance of an obstacle (0, 1, 2, 3, 4) to the PC
			state_value = 5'b00100;
			tx_data = check_distance;
			if (tx_ready) begin
				nextstate = received_message;
				tx_valid = 1;
			end else if(timeout_count >= TIMEOUT) begin
			    nextstate = received_message;
			end else begin
				tx_valid = 0;
				nextstate = sending_message_sensor;
			end
		end

		received_message: begin //checks if the zigbee received new information from the PC
			state_value = 5'b00101;
			rx_ready = 1;

			if (rx_valid)
			    nextstate = decission_direction;
			else
				nextstate = received_message;
		end

		decission_direction: begin //chooses a direction based on the incoming UART signal
			state_value = 5'b00110;

			if (rx_data == 8'b01000110) // forward (F)
			    nextstate = line_follower_activate;
			else if (rx_data == 8'b01001100) // left (L)
			    nextstate = left_central;
			else if (rx_data == 8'b01010010) // right (R)
			    nextstate = right_central;
			else if (rx_data == 8'b01000010) // turn around (B)
					nextstate = left_central;
			else if (rx_data == 8'b01010000) // backwards (P)
				nextstate = backward_central;
			else
			    nextstate = decission_direction;

		end

		left_central: begin //the robot goes left until the sensors see 101 again 
			state_value = 5'b00111;
			if (sensor_l == 1 && sensor_m == 0 && sensor_r == 1 && off_line == 1)
				if (rx_data == 8'b01000010) begin
					nextstate = off_line_reset;
				end else
					nextstate = brake_state_2;
			else
				nextstate = left;

		end

		left: begin //the robot makes a left turn
			state_value = 5'b01000;
			motor_l_reset = 0;
            motor_r_reset = 0;
            motor_l_direction = 0;
            motor_r_direction = 0;
            count_reset = 0;

            if (count_in >= 21'd2_000_000)
                nextstate = left_central;

		end

		right_central: begin //the robot goes right until the sensors see 101 again 
			state_value = 5'b01001;
			if (sensor_l == 1 && sensor_m == 0 && sensor_r == 1 && off_line == 1)
				nextstate = brake_state_2;
			else
				nextstate = right;

		end

		right: begin //the robot makes a right turn
			state_value = 5'b01010;
			motor_l_reset = 0;
            motor_r_reset = 0;
            motor_l_direction = 1;
            motor_r_direction = 1;
            count_reset = 0;
                	
            if (count_in >= 21'd2_000_000)
                nextstate = right_central;

		end
		
		off_line_reset: begin
			state_value = 5'b01011;
		    nextstate = turnaround_central;
		    
		end
		
		turnaround_central: begin //the robot goes right until the sensors see 101 again 
			state_value = 5'b01100;
			if (sensor_l == 1 && sensor_m == 0 && sensor_r == 1 && off_line == 1)
				nextstate = brake_state_2;
			else
				nextstate = turnaround;

		end
	
		turnaround: begin
			state_value = 5'b01101;
			motor_l_reset = 0;
            motor_r_reset = 0;
            motor_l_direction = 0;
            motor_r_direction = 0;
            count_reset = 0;

            if (count_in >= 21'd2_000_000)
                nextstate = turnaround_central;

		end

		backward_central: begin //the robot goes back until the sensors see 111
			state_value = 5'b01110;
			if (sensor_l == 1 && sensor_m == 1 && sensor_r == 1) begin
				nextstate = brake_state_2;
			end
			else
				nextstate = backward;

		end

		backward: begin //the robot goes backwards
			state_value = 5'b01111;
			motor_l_reset = 0;
            motor_r_reset = 0;
            motor_l_direction = 0;
            motor_r_direction = 1;
            count_reset = 0;

            if (count_in >= 21'd2_000_000)
                nextstate = backward_central;

		end

		brake_state_2: begin //when brake equals 1 the PWM signal is 1.5ms, which causes the robot to brake
			state_value = 5'b10000;
			motor_l_reset = 0;
			motor_r_reset = 0;
			brake = 1;
			count_reset = 0;
			activate_sensor = 1;

			if (count_in >= 21'd2_000_000)
				nextstate = sending_message_2;
		end

		sending_message_2: begin //sends a + to the PC
			state_value = 5'b10001;
			tx_data = 8'b00101011;
            if (tx_ready) begin
				tx_valid = 1;
				nextstate = sending_message_sensor_2;
			end else begin
				tx_valid = 0;
				nextstate = sending_message_2;
			end
		end

		sending_message_sensor_2: begin //sends the distance of an obstacle (0, 1, 2, 3, 4) to the PC
			state_value = 5'b10010;
			tx_data = check_distance;
			if (tx_ready) begin
				tx_valid = 1;
				nextstate = received_message;
			end else begin;
				tx_valid = 0;
				nextstate = sending_message_sensor_2;
			end
		end

        line_follower_activate: begin // activates the line follower module
			state_value = 5'b10011;
            line_follower_start = 1;

            nextstate = IDLE;

		end
		
		default: begin
		   	nextstate = IDLE;
           	motor_l_reset = 1;
           	motor_r_reset = 1;
           	count_reset = 1;
		   	motor_l_direction = 0;
			motor_r_direction = 0;
			brake = 0;
			rx_ready = 0;
  			tx_valid = 0;
  			tx_data = 8'h00;
			line_follower_start = 0;
			activate_sensor = 0;
        end
	endcase
	end
endmodule