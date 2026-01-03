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

#endif // LCD_WORDS_H
