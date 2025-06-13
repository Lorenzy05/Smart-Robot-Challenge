module  sonar_sensor(
    input logic clk, //100 Mhz
    input logic reset, //active-low 
    input logic echo,
    input logic activate_sensor,

    input logic [23:0] count_trigger,
    input logic [23:0] count_echo,
    input logic [23:0] count_cycle,

    output logic reset_count_trigger,
    output logic reset_count_echo,
    output logic reset_count_cycle,
    output logic en_trigger,
    output logic en_echo,
    output logic en_cycle,
    
    output logic trigger,
    output logic [7:0] check_distance

    );
    
    /*output logic check_distance
    0= nothing
    1= object on grid 1
    2= object on grid 2
    3= object on grid 3
    4= object on grid 4
    5= no object found
    
    , WAIT_CYCLE
    */

    typedef enum{IDLE, TRIGGERING, ECHO, CALCULATE} state_t;

    state_t state , nextstate;

    always_ff @ (posedge clk) begin
        if (reset)
            state <= IDLE;
        else
            state <= nextstate;
    end

    always_comb begin
        nextstate = state;

        trigger = 0;
        reset_count_trigger = 0;
        en_trigger = 0;
        reset_count_echo = 0;
        en_echo = 0;
        reset_count_cycle = 0;
        en_cycle = 1;

        check_distance = 8'b00110000; // 0

        unique case (state)

            IDLE: begin
                if (activate_sensor) begin
                    reset_count_cycle = 1;
                    reset_count_echo = 1;  
                    reset_count_trigger = 1; 
                    nextstate = TRIGGERING;
                end
            end

            TRIGGERING: begin
                trigger = 1;
                en_trigger = 1;
                if(count_trigger>=24'd1000)
                    nextstate = ECHO;
            end

            ECHO: begin
                if(echo) begin
                    nextstate = CALCULATE;
                end                    
            end
            CALCULATE: begin
                en_echo = 1;

                if(!echo)begin // problem with echo (echo wont come down)
                    en_echo = 0;
                    
                    if(count_echo <= 24'd252300 && count_echo >=24'd0000000 )
                        check_distance= 8'b00110001; //grid 1 (1)
                    else if( count_echo <= 24'd519100 && count_echo >=24'd252300)
                        check_distance= 8'b00110010; //grid 2 (2)
                    else if(count_echo <= 24'd758640 && count_echo >=24'd519100 )
                        check_distance= 8'b00110011; //grid 3 (3)
                    else if(count_echo <= 24'd979040 && count_echo >=24'd758640 )
                        check_distance= 8'b00110100; //grid 4 (4)
                    else
                        check_distance= 8'b00110000; // 0
                        
                      //if(count_echo <= 24'd61248)
                            //check_distance = 3'b001;
                      //else
                        //check_distance = 3'b101;

 
                       
                      
                if(count_cycle >= 24'd10_000_000)
                    nextstate = IDLE;
                end
            end
            
            default: begin
                nextstate = IDLE;
            end
        endcase
    end    
endmodule
