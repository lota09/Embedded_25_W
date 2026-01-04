/*******************************************************************
  - Project          : 2025 Embedded SW
  - File name        : lcd_words.c
  - Description      : list of word expression for hangman game
  - Owner            : Seokmin Kang
  - Revision history : 1) 2025.12.31 : Initial release 
*******************************************************************/

#ifndef LCD_WORDS_H
#define LCD_WORDS_H

#include <avr/pgmspace.h>

const char word0[] PROGMEM = "HELLO";
const char word1[] PROGMEM = "WORLD";
const char word2[] PROGMEM = "APPLE";
const char word3[] PROGMEM = "BRAIN";
const char word4[] PROGMEM = "CLOCK";
const char word5[] PROGMEM = "DRIVE";
const char word6[] PROGMEM = "EAGLE";
const char word7[] PROGMEM = "FLAME";
const char word8[] PROGMEM = "GRAPE";
const char word9[] PROGMEM = "HOUSE";

const char* const words_table[] PROGMEM = {
    word0, word1, word2, word3, word4, 
    word5, word6, word7, word8, word9
};

void lcd_init();
void lcd_clear();
void lcd_puts(char *str);
void lcd_gotoxy(uint8_t x, uint8_t y);
void lcd_clearline(bool line);
void newword(int index);
bool inferword(char c);
bool is_wordcomplete(void);

#endif // LCD_WORDS_H
