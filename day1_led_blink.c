/*******************************************************************
  - Project          : 2025 Embedded SW
  - File name        : day1_led_blink.c
  - Description      : Blink LED
  - Owner            : Seokmin Kang
  - Revision history : 1) 2025.12.23 : Initial release 
*******************************************************************/

#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	DDRB |= 0b00000010;
	
    /* Replace with your application code */
    while (1) 
    {
		PORTB |= 0b00000010;
		_delay_ms(100);
		
		PORTB &= 0b11111101;
		_delay_ms(500);	
        
        //PINB |= 0b00000010;
        
    }
}

