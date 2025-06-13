module inputbuffer
   (input logic clk,
    input logic sensor_l_in,
    input logic sensor_m_in, 
    input logic sensor_r_in,
    output logic sensor_l_out,
    output logic sensor_m_out, 
    output logic sensor_r_out);

	logic [2:0] sensor_vector_input;
	logic [2:0] sensor_vector_mid;
	logic [2:0] sensor_vector_output;
	assign sensor_vector_input = {sensor_l_in, sensor_m_in, sensor_r_in};

	always_ff @(posedge clk)
		sensor_vector_mid <= sensor_vector_input;

	always_ff @(posedge clk)
		sensor_vector_output <= sensor_vector_mid;

	assign {sensor_l_out, sensor_m_out, sensor_r_out} = sensor_vector_output;

endmodule
