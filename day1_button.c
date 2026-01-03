/*******************************************************************
  - Project          : 2025 Embedded SW
  - File name        : day1_button.c
  - Description      : Button LED
  - Owner            : Seokmin Kang
  - Revision history : 1) 2025.12.24 : Initial release 
*******************************************************************/

#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	DDRB |= 0b00000010; //Writemode PB1
  DDRA &= 0b11111100; //Readmode PA0, PA1

  PORTA |= 0b00000011; //Pullup PA0, PA1
	
    /* Replace with your application code */
    while (1) 
    {
      if(PINA&1)
        PORTB &= 0b11111101;
      else
        PORTB |= 0b00000010;

      if(PINA&2)
        PORTB &= 0b11111011;
      else
        PORTB |= 0b00000100;
    }
}
