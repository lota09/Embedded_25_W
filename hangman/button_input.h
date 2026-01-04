/*******************************************************************
  - Project          : 2025 Embedded SW
  - File name        : button.c
  - Description      : Button logic for hangman game 
  - Owner            : Seokmin Kang
  - Revision history : 1) 2025.12.31 : Initial release 
*******************************************************************/

#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>

#define BUTTON_DDR DDRA
#define BUTTON_PORT PORTA
#define BUTTON_PIN PINA
#define BUTTON_BIT 0

static void button_init(void)
{
    BUTTON_DDR &= ~(1 << BUTTON_BIT); // Button pin as input
    BUTTON_PORT |= (1 << BUTTON_BIT); // Enable pull-up resistor
}

static uint8_t button_read(void)
{
    return (BUTTON_PIN & (1 << BUTTON_BIT)) == 0;
}

int main(void)
{
    button_init();
    
    while (1)
    {
        if (button_read())
        {
            // Button is pressed
        }
    }
}
