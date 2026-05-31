#include "console.h"


/**
 * Instantiate classes
 */
LAN     console_lan;
APP     app_function;
I2C     i2c_function;
NVM     nvm_function;


char        console_buffer[PREF_BUFF_ELEMENTS]              = {NULL};
char        console_ssid[PREF_BUFF_ELEMENTS]                = {NULL};
char        console_wifi_pass[PREF_BUFF_ELEMENTS]           = {NULL};

uint8_t     user_option                                 = 0;
uint8_t     temp_uint8t                                 = 0;    
uint8_t     save_0x55                                   = 0x55;
uint16_t    fuel_gauge_flags                            = 0x0000;

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

void CONSOLE::console ( Preferences & pref, APP & app_functions )  
{

    String test_read_string = "";

    clear_screen(); //Don't want to run insid the while  
    
    while(user_option != 99)
    {
        Serial.println("1)  Print SW version.");
        Serial.println("2)  Print HW version.");
        Serial.println("3)  To send test email.");
        Serial.println("4)  View RH calibration values.");
        Serial.println("5)  View network parameters.");
        Serial.println("6)  Vew battery state of charge.");
        Serial.println("7)  View sensor readings.");
        Serial.println("8)  Perform NVM test.");
        Serial.println("9)  Read the state of the push button.");
        Serial.println("10) Read the busy input as sourced by the display.");
        Serial.println("11) Look at charge enable bit.");
        Serial.println("12) Enther network parameters.");
        Serial.println("13) Overwrite relative humidity offsets.");
        Serial.println("14) Enter temperature offsets.");
        Serial.println("15) Enable/disable email sending.");
        Serial.println("16) View BQ27427 (LiIon Fuel gauge) flags.");
        Serial.println("17) Define the battery's capacity.");
        Serial.println("18) Turn display power ON.");
        Serial.println("19) Turn display power OFF.");
        Serial.println("20) Turn IO Expander ON.");
        Serial.println("21) Turn IO Expander OFF.");

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

                app_function.gpio_expander_on();
                delay(10);
                Serial.print("HW revision: ");
                Serial.println(i2c_function.get_hw_revision()); 
                app_function.gpio_expander_off();


                insert_line_emphasis();
            break;
                
            /************************************/
            /* Send test email */
            /************************************/
            case 3:
                clear_screen();
                insert_line_feeds(2);
                insert_line_emphasis();
                
                if(nvm_function.network_valid(pref))
                {

                    /**
                    * Clear the temporary
                    * character buffer just to be 
                    * safe
                    */
                    memset(console_ssid, NULL, sizeof(console_ssid));
                    
                    /**
                     * Get the SSID of the router 
                     */
                    nvm_function.nvm_read_string(pref, PREF_WIFI_SSID, console_ssid);

                    
                    /**
                    * Clear the temporary
                    * character buffer just to be 
                    * safe
                    */
                    memset(console_wifi_pass, NULL, sizeof(console_wifi_pass));
                    
                    /**
                     * Get the router's password 
                     */
                    nvm_function.nvm_read_string(pref, PREF_WIFI_PASSWORD, console_wifi_pass);

                    console_lan.WiFiConnect(console_ssid,console_wifi_pass);  
                    console_lan.send_email(pref);  

                }
                else {
                    Serial.println("Network parameters are invalid");
                }

                
                insert_line_emphasis();
                insert_line_feeds(2);
            break;
                
            /************************************/
            /* View RH calibration values */
            /************************************/
            case 4:
                clear_screen();
                insert_line_feeds(2);
                insert_line_emphasis();

                Serial.print("Value stored for RH offset: ");
                Serial.println(nvm_function.nvm_get_float(pref,PREF_RH_OFFSET));
                
                insert_line_emphasis();
                insert_line_feeds(2);
            break;
                    
            /************************************/
            /* View network parameters */
            /************************************/
            case 5:
                clear_screen();
                insert_line_feeds(2);
                insert_line_emphasis();
                
                /**
                * Clear the temporary
                * character buffer just to be 
                * safe
                */
                memset(console_buffer, NULL, sizeof(console_buffer));
                
                /**
                 * Check the WIFI's SSID
                 */
                nvm_function.nvm_read_string(pref, PREF_WIFI_SSID, console_buffer);
                Serial.print("WiFi SSID: ");
                Serial.println(console_buffer);
                
                /**
                 * Clear the temporary
                 * character buffer just to be 
                 * safe
                 */
                memset(console_buffer, NULL, sizeof(console_buffer));
                
                /**
                 * Check the WIFI's 
                 */
                nvm_function.nvm_read_string(pref, PREF_WIFI_PASSWORD, console_buffer);
                Serial.print("WiFi password: ");
                Serial.println(console_buffer);
                
                /**
                 * Clear the temporary
                 * character buffer just to be 
                 * safe
                 */
                memset(console_buffer, NULL, sizeof(console_buffer));
                
                /**
                 * Check author's email address 
                 */
                nvm_function.nvm_read_string(pref, PREF_EMAIL_AUTHOR_KEY, console_buffer);
                Serial.print("Sender email address: ");
                Serial.println(console_buffer);
                
                /**
                 * Clear the temporary
                 * character buffer just to be 
                 * safe
                 */
                memset(console_buffer, NULL, sizeof(console_buffer));
                
                /**
                 * Check the recipient email address
                 */
                nvm_function.nvm_read_string(pref, PREF_EMAIL_RECIPIENT_KEY, console_buffer);
                Serial.print("Recipient email address: ");
                Serial.println(console_buffer);
                
                /**
                 * Clear the temporary
                 * character buffer just to be 
                 * safe
                 */
                memset(console_buffer, NULL, sizeof(console_buffer));
                
                /**
                 * Check the author's email password
                 */
                nvm_function.nvm_read_string(pref, PREF_EMAIL_AUTHOR_PASSWORD_KEY, console_buffer);
                Serial.print("Password of sending email address: ");
                Serial.println(console_buffer);
                
                insert_line_emphasis();
                insert_line_feeds(2);

            break;
                
            /************************************/
            /* View battery voltage */
            /************************************/
            case 6:
                clear_screen();
                insert_line_feeds(2);
                insert_line_emphasis();

                Serial.print("Battery state of charge: ");
                Serial.println(i2c_function.batt_sen_soc(FILTERED));

                insert_line_emphasis();
                insert_line_feeds(2);
            break;

            /************************************/
            /* Read values from sensors */
            /************************************/
            case 7:
                clear_screen();
                insert_line_feeds(2);
                insert_line_emphasis();

                i2c_function.get_sensor_data(pref); 

                Serial.print("Humidity #1: ");
                Serial.println(i2c_function.hum_val1);
                Serial.print("Temperature #1: ");
                Serial.println(i2c_function.temp_val1);
                Serial.println(' ');

                insert_line_emphasis();

                Serial.print("Humidity #2: ");
                Serial.println(i2c_function.hum_val2);
                Serial.print("Temperature #2: ");
                Serial.println(i2c_function.temp_val2);

                insert_line_emphasis();
                insert_line_feeds(2);
            break;
            
            /************************************/ 
            /* Perform NVM test */ 
            /************************************/ 
            case 8:
                clear_screen();
                insert_line_feeds(2);
                insert_line_emphasis();

                Serial.print("Writing this byte to the NVM:");  
                Serial.println(save_0x55, HEX);  
                
                nvm_function.nvm_store_byte(pref, NVM_NAMESPACE, save_0x55);

                delay(100);
                
                temp_uint8t = nvm_function.nvm_read_byte(pref,NVM_NAMESPACE);

                Serial.print("Value read back from NVM:");
                Serial.println(temp_uint8t, HEX);

                if(temp_uint8t ==  save_0x55 ) 
                { 
                    Serial.println("NVM test passed! ");
                }
                else 
                {
                    Serial.println("NVM test failed.");
                }

                insert_line_emphasis();
                insert_line_feeds(2);
            break;
            
            /************************************/
            /* Read the state of the push button */
            /************************************/
            case 9:
                clear_screen();
                insert_line_feeds(2);
                insert_line_emphasis();
                
                pinMode(BUTTON_INPUT,INPUT);


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
            /* Read the display's busy flag */
            /************************************/ 
            case 10:
                clear_screen();
                insert_line_feeds(2);
                insert_line_emphasis();
                
                pinMode(BUSY_PIN,INPUT);


                Serial.print("The current state of the busy pin is: ");  
                
                if(digitalRead(BUSY_PIN)) 
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
            /* Look at charging bit */ 
            /************************************/
            case 11:
                clear_screen();
                insert_line_feeds(2);
                insert_line_emphasis();
                
                app_function.gpio_expander_on();
                delay(10);

                if(i2c_function.charging_is_active())
                {
                    Serial.println("Charging active");  
                }
                else
                {
                    Serial.println("Charging inactive");  
                }

                app_function.gpio_expander_off();
                
                insert_line_emphasis();
                insert_line_feeds(2);
                
            break;
            
            /************************************/
            /** Enter network parameters */
            /************************************/
            case 12:
            {
                // Some links that we may want to check out to 
                // get this preference string store stuff to work 
                // https://espressif-docs.readthedocs-hosted.com/projects/arduino-esp32/en/latest/api/preferences.html
                // https://docs.espressif.com/projects/arduino-esp32/en/latest/tutorials/preferences.html#:~:text=The%20Preferences%20library%20is%20unique,power%20events%20to%20the%20system.
                //

                clear_screen();
                insert_line_feeds(2);
                insert_line_emphasis();
                
                /**
                 * Clear the temporary buffer
                 */
                memset(console_buffer, NULL, sizeof(console_buffer));

                /**
                 * Get WIFI SSID
                 */
                Serial.print("Enter WIFI SSID: ");
                get_char_buffer_from_user(console_buffer);
                Serial.println();
                
                if(CONSOLE_LOGGING)
                {
                    Serial.print("^User entered: ");
                    Serial.println(console_buffer);
                }
                
                /**
                 * Store WIFI password
                 */
                nvm_function.nvm_store_string(pref, PREF_WIFI_SSID, console_buffer);


                /**
                 * Clear the temporary buffer
                 */
                memset(console_buffer, NULL, sizeof(console_buffer));
                
                /**
                 * Get WIFI password
                 */
                Serial.print("Enter WIFI password: ");
                get_char_buffer_from_user(console_buffer);
                Serial.println();
                
                if(CONSOLE_LOGGING)
                {
                    Serial.print("^User entered: ");
                    Serial.println(console_buffer);
                }
                
                /**
                 * Store the WIFI password
                 */
                nvm_function.nvm_store_string(pref, PREF_WIFI_PASSWORD, console_buffer);
                
                
                /**
                 * Clear the temporary buffer
                 */
                memset(console_buffer, NULL, sizeof(console_buffer));
                
                /**
                 * Get author's email address
                 * Get character buffer from user
                 */
                Serial.print("Enter the sender's email address: ");
                get_char_buffer_from_user(console_buffer);
                Serial.println();

                if(CONSOLE_LOGGING)
                {
                    Serial.print("^User entered: ");
                    Serial.println(console_buffer);
                }
                
                /**
                 * Store the author's email 
                 * address
                 */
                nvm_function.nvm_store_string(pref, PREF_EMAIL_AUTHOR_KEY, console_buffer);
                
                /**
                 * Clear the temporary buffer
                 */
                memset(console_buffer, NULL, sizeof(console_buffer));
                
                /**
                 * Get author's email password
                 */
                Serial.print("Enter password of the sending email address: ");
                get_char_buffer_from_user(console_buffer);
                Serial.println();

                if(CONSOLE_LOGGING)
                {
                    Serial.print("^User entered: ");
                    Serial.println(console_buffer);
                }
                
                /**
                 * Store the password for the author's email
                 */
                nvm_function.nvm_store_string(pref,PREF_EMAIL_AUTHOR_PASSWORD_KEY, console_buffer);

                /**
                 * Clear the temporary buffer
                 */
                memset(console_buffer, NULL, sizeof(console_buffer));
                
                /**
                 * Get recipient's email address
                 */
                Serial.print("Enter the recipient's email address: ");
                get_char_buffer_from_user(console_buffer);
                Serial.println();

                if(CONSOLE_LOGGING)
                {
                    Serial.print("^User entered: ");
                    Serial.println(console_buffer);
                }
                
                /**
                 * Store the recipient's email address
                 */
                nvm_function.nvm_store_string(pref, PREF_EMAIL_RECIPIENT_KEY, console_buffer);

                insert_line_emphasis();
                insert_line_feeds(2);
                
            }
            break;

            /************************************/
            /* Overwrite RH Offset Values */
            /************************************/
            case 13:
                clear_screen();
                insert_line_feeds(2);
                insert_line_emphasis();
                
                /**
                 * Clear the temporary buffer
                 */
                memset(console_buffer, NULL, sizeof(console_buffer));

                Serial.println("User wishes to overwrite relative humidity offset values.");
                Serial.println("Relative humidity offset values are positive numbers.");
                Serial.print("Are you absolutely sure you want to do this (y/n): ");
                get_char_buffer_from_user(console_buffer);
                Serial.println('\n');
                
                
                if(console_buffer[0] == 'y' || console_buffer[0] =='Y')
                {
                    Serial.println("Now modifying RH offset value.");
                    Serial.print("What is the offset (enter a negative offset here if reading is too high): ");
                    temp_float = get_user_float_value();
                    Serial.println();
                    nvm_function.nvm_store_float(pref, PREF_RH_OFFSET, temp_float);
                    
                    nvm_function.nvm_store_int(pref, PREF_CAL_KEY, VALID_CAL_VALUE);
                    app_functions.calibration_just_occurred = true;
                }
                
                if(CONSOLE_LOGGING)
                {
                    Serial.print("^Value entered for RH offset: ");
                    Serial.println(nvm_function.nvm_get_float(pref,PREF_RH_OFFSET));
                }

                
                insert_line_emphasis();
                insert_line_feeds(2);
                break;
                
            /************************************/
            /* Enter Temperature Offsets */
            /************************************/
            case 14:
                clear_screen();
                insert_line_feeds(2);
                insert_line_emphasis();
                
                /**
                 * Clear the temporary buffer
                 */
                memset(console_buffer, NULL, sizeof(console_buffer));

                Serial.println("User wishes to enter temperature offsets.");
                
                Serial.print("Enter the offset value for temperature (two will be set to the same): ");
                temp_float = get_user_float_value();
                Serial.println();
                nvm_function.nvm_store_float(pref, PREF_TEMP_OFFSET, temp_float);
                
                i2c_function.temp_offset = temp_float;
                
                if(CONSOLE_LOGGING)
                {
                    Serial.print("^Value entered for temp offset: ");
                    Serial.println(nvm_function.nvm_get_float(pref,PREF_TEMP_OFFSET));
                }
                
                insert_line_emphasis();
                insert_line_feeds(2);
            break;

            /************************************/
            /* Enable/disable the sending of emails
            /************************************/
            case 15:
                clear_screen();
                insert_line_feeds(2);
                insert_line_emphasis();
                
                if(console_lan.email_enabled) 
                {
                    Serial.println("The email feature is currently enabled");
                }
                else 
                {
                    Serial.println("The email feature is currently disabled");
                }
                
                /**
                 * Clear the temporary buffer
                 */
                memset(console_buffer, NULL, sizeof(console_buffer));
                
                
                /**
                 * Get author's email password
                 */
                Serial.print("Do you wish to enable the email feature? (Y/N): ");
                get_char_buffer_from_user(console_buffer);
                Serial.println();
                
                if(console_buffer[0] == 'y' || console_buffer[0] =='Y')
                {
                    console_lan.email_enabled = true;  
                }
                
                if(console_lan.email_enabled) 
                {
                    Serial.println("The email feature is currently enabled");
                }
                else 
                {
                    Serial.println("The email feature is currently disabled");
                }

                insert_line_emphasis();
                insert_line_feeds(2);
                break;
            
            /************************************/
            /* Read the flags of the fuel gauge */
            /************************************/
            case 16:
                clear_screen();
                insert_line_feeds(2);
                insert_line_emphasis();

                /**
                 * The battery detect flag is bit #3
                 */
                fuel_gauge_flags = i2c_function.batt_sen_get_flags();
                Serial.print("Fuel gauge flags: ");
                i2c_function.print16b_bin(fuel_gauge_flags);
                Serial.println();

                /**
                 * Print the battery detect bit
                 */
                Serial.print("The battery detect flag is: ");
                Serial.println((fuel_gauge_flags >> 3) & 0x01);
                
                /**
                 * Print SOCF flag
                 * this is defined as the second or final capacity threshold
                 */
                Serial.print("The final SOC [SOCF] bit is: ");
                Serial.println((fuel_gauge_flags >> 1) & 0x01);
                
                /**
                 * Print SOC1 flag
                 * this is defined as the first capacity threshold
                 */
                Serial.print("The first [SOC1] bit is: ");
                Serial.println((fuel_gauge_flags >> 2) & 0x01);


                insert_line_emphasis();
                insert_line_feeds(2);
            break;
            
            /************************************/
            /* Define Battery Capacity
            /************************************/
            case 17:
                clear_screen();
                insert_line_feeds(2);
                insert_line_emphasis();

                Serial.print("Enter the battery's capacity (in mAh): ");
                temp_float = get_user_float_value();
                Serial.println();
                Serial.print("The user entered: ");
                Serial.print(temp_float);
                Serial.println(" mAh");
                i2c_function.batt_sen_set_capacity((uint16_t)temp_float);
                Serial.println("Battery capacity has been updated.");
                
                insert_line_emphasis();
                insert_line_feeds(2);
                break;
        
            /************************************/
            /* Turn display power ON
            /************************************/
            case 18:
                clear_screen();
                insert_line_feeds(2);
                insert_line_emphasis();

                app_function.gpio_expander_on();
                delay(10);
                i2c_function.set_io_expander(1, false);    // Power EN is active low 
                
                insert_line_emphasis();
                insert_line_feeds(2);
                break;
                
                /************************************/
                /* Turn display power OFF
                /************************************/
                case 19:
                clear_screen();
                insert_line_feeds(2);
                insert_line_emphasis();
                
                i2c_function.set_io_expander(1, true);    // Power EN is active low 
                app_function.gpio_expander_off();
                
                insert_line_emphasis();
                insert_line_feeds(2);
                break;
                
            /************************************/
            /* Turn GPIO Expander ON
            /************************************/
            case 20:
                clear_screen();
                insert_line_feeds(2);
                insert_line_emphasis();
    
                app_function.gpio_expander_on();
                
                insert_line_emphasis();
                insert_line_feeds(2);
            break;
            
            /************************************/
            /* Turn GPIO Expander OFF
            /************************************/
            case 21:
                clear_screen();
                insert_line_feeds(2);
                insert_line_emphasis();
    
                app_function.gpio_expander_off();
                
                insert_line_emphasis();
                insert_line_feeds(2);
            break;


            /************************************/
            /* Exit the application */
            /************************************/
            case 99:
                app_function.full_screen_refresh(pref);
                clear_screen();
                Serial.print("Exiting terminal...");
            break;

            default:
                __asm__("nop\n\t");
            break;

    }


    }



}