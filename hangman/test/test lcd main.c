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

// Function Prototypes from lcd_letters.c
void lcd_init();
void lcd_clear();
void lcd_puts(char *str);
void lcd_gotoxy(uint8_t x, uint8_t y);
void newword(int index);
bool inferword(char c);
bool is_wordcomplete(void);

// For testing purposes, we include the c file if not compiling separately.
// If your environment compiles all .c files automatically, comment this out.
#include "lcd_letters.c" 

// Needed for OLED if we want to keep it running, but focused on LCD now as requested.

int main(void)
{
    // Initialize
    lcd_init(); 
    lcd_puts("Hangman Test");
    _delay_ms(1000);
    
    // Test Array: Characters to try guessing
    // 'H', 'E', 'L', 'L', 'O' match "HELLO" (Word 0)
    // 'Z', 'X' are wrong guesses
    char test_guesses[] = "ELZLOXH"; 
    
    while (1)
    {
        newword(0); // Load "HELLO"
        _delay_ms(1000);
        
        int i = 0;
        // Loop until word is complete OR we run out of test characters
        while (!is_wordcomplete() && test_guesses[i] != '\0')
        {
            lcd_gotoxy(0, 1); // Move to 2nd line for status
            lcd_puts("                "); // Clear line
            lcd_gotoxy(0, 1);
            
            char c = test_guesses[i];
            bool result = inferword(c);
            
            char buffer[17];
            // Format: "H : True" or "Z : False"
            lcd_gotoxy(0, 1);
            sprintf(buffer, "%c : %s", c, result ? "True" : "False");
            lcd_puts(buffer);
            
            _delay_ms(1000); 
            i++;
        }
        
        lcd_gotoxy(0, 1);
        lcd_puts("                "); // Clear line
        lcd_gotoxy(0, 1);
        
        if (is_wordcomplete()) {
            lcd_puts("Word Complete!");
        } else {
             lcd_puts("Test Failed.");
        }
        
        _delay_ms(3000);
    }
}
