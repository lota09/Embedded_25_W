/*******************************************************************
  - Project          : 2025 Embedded SW
  - File name        : oled_life.c
  - Description      : OLED display bitmap and header for hangman game 
  - Owner            : Seokmin Kang
  - Revision history : 1) 2025.12.31 : Initial release 
*******************************************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#ifndef OLED_LIFE_H
#define OLED_LIFE_H

#define OLED_CS_DDR DDRB
#define OLED_CS_PORT PORTB
#define OLED_CS_BIT 0

#define OLED_DC_DDR DDRB
#define OLED_DC_PORT PORTB
#define OLED_DC_BIT 1

static uint8_t life;

// 16x16 Heart Bitmap (Page Addressing: 2 Pages x 16 Columns)
// Page 0 (Top), Page 1 (Bottom)
const uint8_t img_heart[] PROGMEM = {
    // Top Half
    0x00, 0x00, 0x00, 0x1C, 0x3E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x3E, 0x1C, 0x00, 0x00, 0x00,
    // Bottom Half
    0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x03, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// Placeholder for GAME OVER text (Fixed width 12px for now)
const uint8_t char_G[] PROGMEM = {0};
const uint8_t char_A[] PROGMEM = {0};
const uint8_t char_M[] PROGMEM = {0};
const uint8_t char_E[] PROGMEM = {0};
const uint8_t char_O[] PROGMEM = {0};
const uint8_t char_V[] PROGMEM = {0};
const uint8_t char_R[] PROGMEM = {0};

/* 
 * Bitmap Array Table
 * 0: Heart, 1: G, 2: A, 3: M, 4: E, 5: O, 6: V, 7: E, 8: R 
 */
const uint8_t * const oled_bitmap_addr[] PROGMEM = {
    img_heart, char_G, char_A, char_M, char_E, char_O, char_V, char_E, char_R
};

// Widths: Heart is 16, others are placeholders
const uint8_t oled_bitmap_width[] PROGMEM = {16, 12, 12, 12, 12, 12, 12, 12, 12};

static void SpiTx(uint8_t data); // static in header?
void sh1106_init(void);
void sh1106_set_location(uint8_t page, uint8_t column);
void sh1106_clear(void);
void sh1106_draw_bitmap(const uint8_t *bitmap, uint8_t width, uint8_t page, uint8_t column);
void display_lives(uint8_t lives);

#endif // OLED_BITMAP_H