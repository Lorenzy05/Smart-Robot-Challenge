module crossing_counter
  (input logic clk,
   input logic reset,
   input logic reset_crossing_counter,
   input logic reset_station,
   input logic sensor_l,
   input logic sensor_m, 
   input logic sensor_r,
   output logic [1:0] crossing_counter);

  typedef enum logic [1:0] {S0, S1, S2} statetype;
  statetype state, nextstate;

  always_ff @(posedge clk) begin
      if (reset) begin
        crossing_counter <= 2'b01;
        state <= S0;
      end
      else if (reset_station) 
        crossing_counter <= 2'b01;
      else if (reset_crossing_counter)
         crossing_counter <= 2'b00;
      else begin
         state <= nextstate;
      // Increment ONLY in S1 and no reset is active
      if (state == S1)
        crossing_counter <= crossing_counter + 1;
    end
  end

  always_comb begin 
    nextstate = state;
    case(state)
      S0: 
        if (sensor_l == 0 && sensor_m == 0 && sensor_r == 0)
          nextstate = S1;  // Detect crossing start (000)
        else
          nextstate = S0;

      S1: 
        nextstate = S2;    // Always move to S2 after S1

      S2: 
        // Exit S2 when sensors normalize (101) OR after crossing (010)
        if (sensor_l == 1 && sensor_m == 0 && sensor_r == 1) 
          nextstate = S0;
        else if (sensor_l == 0 && sensor_m == 1 && sensor_r == 0)
          nextstate = S0;
        else
          nextstate = S2;
    endcase
  end
endmodule