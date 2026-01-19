#ifndef OLED_ALPHABET_H
#define OLED_ALPHABET_H

#define F_CPU 8000000

#include <avr/pgmspace.h>
#include <stdint.h>

// OLED 초기화 (SH1106)
void oled_init(void);

// 화면 지우기
void oled_clear(void);

// 문자 하나 출력 (A-Z)
void oled_putc(char c, uint8_t page, uint8_t column);

// 4배 확대 및 스크롤 출력
void oled_putc_scaled(char c);

#endif // OLED_ALPHABET_H