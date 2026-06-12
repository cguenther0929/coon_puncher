#include "console.h"


/**
 * Instantiate classes
 */
APP     console_app_functions;
SENSOR  console_sensor_functions;



uint8_t     user_option                                 = 0;
uint8_t     temp_uint8t                                 = 0;    

float       temp_float                                  = 0;    
float       temporary_voltage_value                     = 0.0;

/**
 * Set to true to 
 * enable logging
 */
#define CONSOLE_LOGGING                false    

void CONSOLE::init(void) 
{

}

 void CONSOLE::flush_serial_input_buffer( void )
 {
    while (Serial.available() > 0) {
        Serial.read();             //gets one byte from serial buffer
    }
 }


/**
 * @brief Get Number Input From User
 */
uint8_t CONSOLE::get_user_uint8t_value ( void )  
{
    uint16_t return_number = 0;
    
    while (Serial.available() <= 0);    // Pause until we start receiving data
    
    Serial.setTimeout(3000);            // Value is in milli-seconds

    char c = Serial.parseInt();         // Parse integer value input by user
    
    return_number = int(c);

    if(return_number > 255) 
    {
        return_number = 255;
    }

    return (return_number);
}

float CONSOLE::get_user_float_value( void )
{
    while (Serial.available() <= 0);    // Pause until we start receiving data
    
    Serial.setTimeout(10000);            // Value is in milli-seconds

    float return_number = Serial.parseFloat();
    
    return (return_number);
}

void CONSOLE::get_char_buffer_from_user(char * char_buffer)
{
    char        received_char   = '\n';
    uint8_t     index           = 0x00;
    
    flush_serial_input_buffer();

    Serial.setTimeout(3000);   // Value is in milli-seconds

    while (Serial.available() <= 0);    // Pause until we start receiving data
    received_char = Serial.read();
    
    while (received_char != 0x0D && index < 255)
    {
        char_buffer[index] = received_char;
        delay(10);
        index++;
        while (Serial.available() <= 0);    // Pause until we receive more data
        received_char = Serial.read();
    }

}

void CONSOLE:: insert_line_feeds( uint8_t spaces ) 
{
    uint16_t i = 0;         //Use this as a counter
	for(i=0;i<spaces;i++) {
		Serial.println('\r');      // Return the cursor
	}
}

void CONSOLE::clear_screen( void ) 
{
    Serial.write(27);       // ESC command
    Serial.print("[2J");    // clear screen command
    Serial.write(27);
    Serial.print("[H");         //Cursor home command
}

void CONSOLE:: cursor_top_left( void ) 
{
    Serial.print("[H");     //Send the rest of the sequence to clear the screen
}

void CONSOLE::insert_line_emphasis( void ) 
{
    Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~");     //Send the rest of the sequence to clear the screen
}

void CONSOLE::user_console ( APP & console_app_functions )  
{

    String test_read_string = "";

    clear_screen(); //Don't want to run insid the while  
    
    while(user_option != 99)
    {
        Serial.println("1)  Print SW version.");
        Serial.println("2)  Print HW version.");   //TODO define as this will be hardcoded for now
        Serial.println("3)  View distance readings.");     
        Serial.println("4)  Read the state of the push button."); 
        Serial.println("5)  Retract cylinder.");  
        Serial.println("6)  Extend cylinder.");   
        Serial.println("7)  Print current threshold.");

        Serial.println("99) To exit the console.");

        Serial.print("Enter a value: ");

        flush_serial_input_buffer();

        user_option = get_user_uint8t_value();  

        if(CONSOLE_LOGGING)      
        {
            Serial.print("^User entered option: ");
            Serial.println(user_option);
        }

        switch (user_option) 
        {
            /************************************/
            /* Print the SW version */
            /************************************/
            case 1:
                clear_screen();
                insert_line_feeds(2);
                insert_line_emphasis();

                Serial.print("The SW version: ");
                Serial.println(SW_VER_STRING);

                insert_line_emphasis();
            break;

            /************************************/
            /* Report the HW version */
            /************************************/
            case 2:           
                clear_screen();
                insert_line_feeds(2);
                insert_line_emphasis();

                // console_app_functions.gpio_expander_on();
                // delay(10);
                // Serial.print("HW revision: ");
                // Serial.println(i2c_function.get_hw_revision()); 
                // console_app_functions.gpio_expander_off();


                insert_line_emphasis();
            break;

            /************************************/
            /* Read distance */
            /************************************/
            case 3:
                clear_screen();
                insert_line_feeds(2);
                insert_line_emphasis();

                
                Serial.println("Taking 30 distance measurements...");
                for(temp_uint8t = 0; temp_uint8t < 30; temp_uint8t++)
                {
                    console_sensor_functions.get_distance();
                    Serial.print("Distance Reading (");
                    Serial.print(temp_uint8t);
                    Serial.print("): ");
                    Serial.println(console_sensor_functions.current_distance);
                    delay(1000);
                }


                insert_line_emphasis();
                insert_line_feeds(2);
            break;
            
            /************************************/
            /* Read the state of the push button */
            /************************************/
            case 4:
                clear_screen();
                insert_line_feeds(2);
                insert_line_emphasis();
                
                Serial.print("The current state of the push button is: ");  
                
                if(digitalRead(BUTTON_INPUT)) 
                {
                    Serial.println("HIGH.");  
                }
                else
                {
                    Serial.println("LOW.");  
                }

                insert_line_emphasis();
                insert_line_feeds(2);
            break;
            
            /************************************/
            /* Retract cylinder 
            /************************************/
            case 5:
            clear_screen();
            insert_line_feeds(2);
            insert_line_emphasis();
            
            console_app_functions.solenoid_stop();
                Serial.println("Retracting the cylinder. ");
                delay(200);
                console_app_functions.solenoid_retract();
                delay(1000);
                console_app_functions.solenoid_stop();
                Serial.println("The cylinder has been retracted. ");
                
                insert_line_emphasis();
                insert_line_feeds(2);
                break;
                
            /************************************/
            /* Extend cylinder 
            /************************************/
            case 6:
            clear_screen();
            insert_line_feeds(2);
            insert_line_emphasis();
            
            Serial.println("Extending the cylinder. ");
            console_app_functions.solenoid_stop();
            delay(200);
            console_app_functions.solenoid_extend();
            delay(1000);
            console_app_functions.solenoid_stop();
            Serial.println("The cylinder has been extended. ");
            
            insert_line_emphasis();
            insert_line_feeds(2);
            break;
            
            /************************************/
            /* Print Current Threshold Value 
            /************************************/
            case 7:
            clear_screen();
            insert_line_feeds(2);
            insert_line_emphasis();
            
            Serial.print("Current threshold setting:");
            Serial.println(console_sensor_functions.threshold_distance);
            
            insert_line_emphasis();
            insert_line_feeds(2);
            break;
            
            /************************************/
            /* Exit the application */
            /************************************/
            case 99:
                clear_screen();
                Serial.print("Exiting terminal...");
            break;

            default:
                __asm__("nop\n\t");
            break;

    }


    }



}