#ifndef HYGROMETERCONSOLE_H
#define HYGROMETERCONSOLE_H

#include <Arduino.h>    //This likely defines wire.h
#include "app.h"



#define SERIAL_STRING_TIMEOUT_MSX10          300


class CONSOLE {
    public:


        /**
         * @brief Console init function
         * @param \p none 
         * @return nothing
         */
        void init(void);

        /**
         * @brief Console routine
         * @details Serves the CLI to the user
         * @param \p none 
         * @return nothing
         */
        void user_console (APP & app_functions);

        /**
         * @brief Get number input from user (0-255)
         * @param \p none 
         * @return the int value that the user entered
         */
        uint8_t get_user_uint8t_value ( void );
        
        /**
         * @brief Get float input from user
         * @param \p none 
         * @return the float value that the user entered
         */
        float get_user_float_value( void );
        
        /**
         * @brief Get character input from user
         * @param \p none 
         * @return the character buffer as defined by the user
         */
       void get_char_buffer_from_user(char * temp_buffer);

        /**
         * @brief For console -- insert spaces
         * @param \p spaces (number of spaces to insert) 
         * @return nothing
         */
        void insert_line_feeds ( uint8_t spaces );

        /**
         * @brief Clear console screen
         * @param \p none 
         * @return nothing
         */
        void clear_screen( void );

        /**
         * @brief Place the cursor at the top-left of the console screen 
         * @param \p none 
         * @return nothing
         */
        void cursor_top_left( void );

        /**
         * @brief Print line emphasis in terminal 
         * @param \p none 
         * @return nothing
         */
        void insert_line_emphasis( void );

        /**
         * @brief Flush serial input buffer 
         * @param \p none 
         * @return nothing
         */
        void flush_serial_input_buffer( void );
        
};

#endif