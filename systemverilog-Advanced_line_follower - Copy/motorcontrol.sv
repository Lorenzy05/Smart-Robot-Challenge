module motorcontrol 
   (input logic clk,
    input logic reset,
    input logic direction, 
	input logic brake,
    input logic [20:0] count_in,
    output logic pwm
	);

	typedef enum logic [1:0] {S0, S1, S2} statetype;
	statetype state, nextstate;

	always_ff @(posedge clk)
		if (reset) state <= S0;
		else       state <= nextstate;
	
	always_comb begin

		pwm = 0;
		nextstate = state;
		
		case (state)
		S0: begin	
			pwm = 0;
			if (~reset)
				nextstate = S1;
		   
			else      
				nextstate = S0;
		    end
		S1: begin	
			if(brake) begin
				pwm = 1;
				if (brake == 1 && count_in >= 21'd150_000)
					nextstate = S2;
			end
			else begin
				pwm = 1;
				if ((direction == 0 && count_in >= 21'd100_000) || (direction == 1 && count_in >= 21'd200_000))      
					nextstate = S2;
			end
		end
		
		S2: begin	
			pwm = 0;
			if (reset) 
				nextstate = S0;
			else        
				nextstate = S2; 
		end
		
		default: begin
			nextstate = S0;
			pwm = 0;
		end
	endcase
	end
endmodule
