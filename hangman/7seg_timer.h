/*******************************************************************
- Project          : 2025 Embedded SW
- File name        : 7seg_timer.h
- Description      : 7 Segment Display and timer logic header for hangman game
- Owner            : Seokmin Kang
- Revision history : 1) 2025.12.24 : Initial release
*******************************************************************/

//// TODO : 인풋이나 다른 로직에 방해를 주지않는 타이머를 만들줄 모름..
#include <avr/io.h>
#include <util/delay.h>

#define USCK_DDR DDRB
#define USCK_PORT PORTB
#define USCK_BIT 7

#define DO_DDR DDRB
#define DO_PORT PORTB
#define DO_BIT 6

#define MAX_CS_DDR DDRB
#define MAX_CS_PORT PORTB
#define MAX_CS_BIT 5


#define INFER_TIMEOUT 15 // second