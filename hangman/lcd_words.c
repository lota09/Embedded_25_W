/*******************************************************************
  - Project          : 2025 Embedded SW
  - File name        : lcd_words.c
  - Description      : lcd display and main game logic for hangman game
  - Owner            : Seokmin Kang
  - Revision history : 1) 2025.12.31 : Initial release 
*******************************************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdbool.h>
#include <avr/pgmspace.h>
#include "lcd_words.h"


// Global variables for game state
char current_word_buffer[17]; // Buffer for the target word (unmasked)
char display_buffer[17];      // Buffer for the LCD display (masked)
bool guessed_mask[16];        // Tracks which letters are guessed
int word_length = 0;

static inline void lcd_wait() {
    _delay_us(40);
}

static void lcd_write_nibble(uint8_t rs, uint8_t data) {
    if(rs) LCD_PORT |= (1 << LCD_RS_BIT);
    else   LCD_PORT &= ~(1 << LCD_RS_BIT);

    LCD_PORT |= (1 << LCD_EN_BIT);  // E High
    LCD_PORT &= 0xF0; 
    LCD_PORT |= (data & 0x0F);
    LCD_PORT &= ~(1 << LCD_EN_BIT); // E Low
}

static void lcd_write_byte(uint8_t rs, uint8_t data) {
    lcd_write_nibble(rs, data >> 4);
    lcd_wait();
    lcd_write_nibble(rs, data & 0x0F);
    lcd_wait(); 
}

void lcd_init() {
    //// RS, E Pin의 방향을 Output으로 설정
    //// DB3~DB0 Pin의 방향을 Output으로 설정
    LCD_DDR |= (1 << LCD_EN_BIT); // E Pin의 방향을 Output으로 설정
    LCD_DDR |= (1 << LCD_RS_BIT);
    LCD_DDR |= 0x0F; // DB3~DB0 Pin의 방향을 Output으로 설정

    _delay_ms(20);
    lcd_write_nibble(0, 3); // Function Set: 8-bit mode
    lcd_wait();
    lcd_write_nibble(0, 2); // Function Set: 4-bit mode
    lcd_wait();
    lcd_write_byte(0, 0b00101000);  // Function Set: 4-bit, 2 lines, 5x8 font
    lcd_wait();
    lcd_write_byte(0, 0b00001100);  // Display On
    lcd_wait();
}

void lcd_clear() {
    lcd_write_byte(0, 0x01);
    _delay_ms(2);
}

void lcd_puts(char *str) {
    while (*str) {
        lcd_write_byte(1, *str++);
    }
}

void lcd_gotoxy(uint8_t x, uint8_t y) {
    uint8_t addr = (y == 0) ? 0x80 : 0xC0;
    addr += x;
    lcd_write_byte(0, addr);
}

void lcd_clearline(bool line)
{
    lcd_gotoxy(0, line);
    lcd_puts("                "); // Clear line
    lcd_gotoxy(0, line); 
}

// --- Hangman Logic ---

void update_word_display() {
    lcd_gotoxy(0, 0); // Line 1
    
    for(int i=0; i<word_length; i++) {
        if(guessed_mask[i]) {
            lcd_write_byte(1, current_word_buffer[i]);
        } else {
            lcd_write_byte(1, '_');
        }
        //lcd_write_byte(1, ' '); // Space for readability
    }
}

void newword(int index) {
    // Reset state
    for(int i=0; i<16; i++) guessed_mask[i] = false;
    
    // Load word from PROGMEM
    strcpy_P(current_word_buffer, (char*)pgm_read_word(&(words_table[index])));
    word_length = strlen(current_word_buffer);
    
    lcd_clear();
    update_word_display();
}

bool inferword(char c) {
    bool found = false;
    for(int i=0; i<word_length; i++) {
        if(current_word_buffer[i] == c || current_word_buffer[i] == (c - 32)) { // Case insensitive check just in case
            if(!guessed_mask[i]) {
                guessed_mask[i] = true;
                found = true;
            }
            // else for dumb found
        }
    }
    
    if(found) {
        update_word_display();
    }
    return found;
}

bool is_wordcomplete(void) {
    for(int i=0; i<word_length; i++) {
        if(!guessed_mask[i]) return false;
    }
    return true;
}
