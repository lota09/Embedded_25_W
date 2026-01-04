/*******************************************************************
  - Project          : 2025 Embedded SW
  - File name        : led_feedback.c
  - Description      : led feedback for hangman game
  - Owner            : Seokmin Kang
  - Revision history : 1) 2025.12.31 : Initial release 
*******************************************************************/

#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>

#define LED_DDR DDRB
#define LED_PORT PORTB
#define LED_PIN PINB
#define LED_RED_BIT 5
#define LED_GREEN_BIT 4
#define LED_WHITE_BIT 3

void led_init();
void led_turnon(uint8_t color);
void led_turnoff(uint8_t color);
void led_blink(uint8_t color, uint8_t count=5, int interval_ms=100);