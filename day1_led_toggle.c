/*******************************************************************
- Project          : 2025 Embedded SW
- File name        : day1_led_toggle.c
- Description      : Toggle LED using PIN register
- Owner            : Seokmin Kang
- Revision history : 1) 2025.12.24 : Initial release
*******************************************************************/

#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>

#define LED_DDR DDRB
#define LED_PORT PORTB
#define LED_PIN PINB
#define LED_BIT 0

int main(void)
{
	DDRB |= 1 << LED_BIT; // Set LED pin as output
	
    /* Replace with your application code */
    while (1) 
    {
        PINB |= 1 << LED_BIT;
        _delay_ms(500); // Wait 500ms
    }
}

