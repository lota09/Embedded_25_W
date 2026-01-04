/*******************************************************************
  - Project          : 2025 Embedded SW
  - File name        : main.c
  - Description      : Top Module for hangman game
  - Owner            : Seokmin Kang
  - Revision history : 1) 2025.12.31 : Initial release 
*******************************************************************/

#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// For testing purposes, we include the c file if not compiling separately.
// If your environment compiles all .c files automatically, comment this out.
#include "lcd_words.h" 
#include "led_feedback.h"

// Needed for OLED if we want to keep it running, but focused on LCD now as requested.

void main_init()
{
    // Initialize
    lcd_init(); 
    lcd_puts("Hangman Test");
    _delay_ms(1000);

    ////TODO
}

void invoke_error(char* error_msg="";)
{
    lcd_clear();
    lcd_gotoxy(0, 0);
    lcd_puts("ERROR OCCURED :");
    lcd_gotoxy(0, 1);
    lcd_puts(error_msg);

    _delay_ms(3000);

    main_init();
}

int main(void)
{
    main_init();

    while (1)
    {
        //// TODO : Determine index Randomly by timer
        newword(0); // Load "HELLO"
        _delay_ms(1000);
        
        int i = 0;
        // Loop until word is complete OR Life give out
        while (!is_wordcomplete() && !is_lost())
        {
            lcd_clearline(1);

            /// TODO : Determine c - User Button Input
            bool result = inferword(c);
            
            char buffer[17];


            switch(result)
            {
                // When Incorrect infer
                case 0:
                    led_blink(LED_RED_BIT);
                    lose_life()
                    sprintf(buffer, "%c : False", c);
                    break;
                // When Correct infer
                case 1:
                    led_blink(LED_GREEN_BIT);
                    sprintf(buffer, "%c : True", c);
                    break;
                // Exception
                default:
                    invoke_error("result not bool");
                    break;
            }

            //Display Infer Result
            lcd_gotoxy(0, 1);
            lcd_puts(buffer);
            
            _delay_ms(1000); 
            i++;
        }
        
        lcd_clearline(1);
        
        if (is_wordcomplete() && !is_lost()) //Player Win
            lcd_puts("You Got It!");
        else if (!is_wordcomplete() && is_lost()) //Player Lost
            lcd_puts("Maybe Nexttime..");
        else // Exception
            invoke_error("win also lost");
        
        _delay_ms(3000);
    }
}
