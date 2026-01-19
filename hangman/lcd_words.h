/*******************************************************************
  - Project          : 2025 Embedded SW
  - File name        : lcd_words.c
  - Description      : list of word expression for hangman game
  - Owner            : Seokmin Kang
  - Revision history : 1) 2025.12.31 : Initial release
*******************************************************************/

#ifndef LCD_WORDS_H
#define LCD_WORDS_H

#define F_CPU 8000000

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdbool.h>
#include <string.h>
#include <util/delay.h>

// LCD 초기화
void lcd_init();

// LCD 화면 지우기
void lcd_clear();

// 문자열 출력
void lcd_puts(char *str);

// 커서 이동
void lcd_gotoxy(uint8_t x, uint8_t y);

// 특정 라인 지우기
void lcd_clearline(bool line);

// 새 단어 로드
void lcd_newword(int index);

// 단어 추측 (문자가 있는지 확인)
bool lcd_inferword(char c);

// 단어 완성 여부 확인
bool lcd_is_wordcomplete(void);

#endif // LCD_WORDS_H
