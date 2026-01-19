/*******************************************************************
  - Project          : 2025 Embedded SW
  - File name        : 7seg_life.c
  - Description      : life counter for hangman game
  - Owner            : Seokmin Kang
  - Revision history : 1) 2025.12.31 : Initial release
*******************************************************************/

#include "7seg_life.h"
#include <avr/io.h>
#include <util/delay.h>

// 19. SCK      > 7SEG CLK, OLED SCK
// 18. DO       > 7SEG DIN, OLED SDA
// 11. PD6      > 7SEG CS

#define MAX_CS_DDR DDRD
#define MAX_CS_PORT PORTD
#define MAX_CS_BIT 6

#define SCK_DDR DDRB
#define SCK_PORT PORTB
#define SCK_BIT 7

#define DO_DDR DDRB
#define DO_PORT PORTB
#define DO_BIT 6

// Life State
static int current_life = 5;

// Internal MAX7219 Functions
static void SpiTx(uint8_t data) {
  USIDR = data;
  for (uint8_t i = 0; i < 8; i++) {
    // USISIE=Unchanged, USIWM=01, USICS=00, USICLK=0, USITC=1
    USICR = (USICR & 0xC0) | 0x11;
    // USISIE=Unchanged, USIWM=01, USICS=00, USICLK=1, USITC=1
    USICR = (USICR & 0xC0) | 0x13;
  }
}

static void max7219_write(uint8_t addr, uint8_t data) {
  MAX_CS_PORT &= ~(1 << MAX_CS_BIT); // CS 핀에 0 출력 (전송 시작)
  SpiTx(addr);                       // CLK과 DIN 핀에 8-bit addr 전송
  SpiTx(data);                       // CLK과 DIN 핀에 8-bit data 전송
  MAX_CS_PORT |= (1 << MAX_CS_BIT);  // CS 핀에 1 출력 (전송 종료)
}

static inline void update_display(int val) { max7219_write(1, val % 10); }

// 공용함수

void seg7_init(void) {
  // Pin Setup
  MAX_CS_DDR |= (1 << MAX_CS_BIT);
  SCK_DDR |= (1 << SCK_BIT);
  DO_DDR |= (1 << DO_BIT);

  max7219_write(0x09, 0x0F); // 뒤 4자리 decode mode
  max7219_write(0x0A, 0x05); // Intensity: 5/32
  max7219_write(0x0B, 0x07); // Scan Limit: 전부표시
  max7219_write(0x0C, 0x01); // Shutdown: Normal
  max7219_write(0x0F, 0x00); // Display Test: Off

  // 모두 지우기
  for (int i = 1; i <= 8; i++) {
    max7219_write(i, 0x0F);
  }

  max7219_write(8, 0x0E); // L
  max7219_write(7, 0x30); // I
  max7219_write(6, 0x47); // F
  max7219_write(5, 0x4F); // E

  current_life = 5;
  update_display(current_life);
}

void seg7_display_lives(void) {
  if (current_life < 0)
    current_life = 0;
  if (current_life > 9)
    current_life = 9;
  update_display(current_life);
}

void seg7_lose_life(void) {
  if (current_life > 0) {
    current_life--;
    update_display(current_life);
  }
}

bool seg7_is_lost(void) { return (current_life == 0); }

void seg7_show_lost(void) {
  max7219_write(0x09, 0x00);

  // 전부 지우기
  for (int i = 1; i <= 8; i++)
    max7219_write(i, 0x00);

  max7219_write(8, 0b00111011); // Y
  max7219_write(7, 0b01111110); // O
  max7219_write(6, 0b00111110); // U
  max7219_write(5, 0x47);       // F
  max7219_write(4, 0b01110111); // A
  max7219_write(3, 0x30);       // I
  max7219_write(2, 0x0E);       // L
  max7219_write(1, 0b10100000); // !
}

void seg7_show_won(void) {
  max7219_write(0x09, 0x00);

  // 전부 지우기
  for (int i = 1; i <= 8; i++)
    max7219_write(i, 0x00);

  max7219_write(8, 0b01011011); // S
  max7219_write(7, 0b00111110); // U
  max7219_write(6, 0b01001110); // C
  max7219_write(5, 0b01001110); // C
  max7219_write(4, 0x4F);       // E
  max7219_write(3, 0b01011011); // S
  max7219_write(2, 0b01011011); // S
  max7219_write(1, 0b10100000); // !
}