/*******************************************************************
  - Project          : 2025 Embedded SW
  - File name        : lcd_words.c
  - Description      : lcd display and main game logic for hangman game
  - Owner            : Seokmin Kang
  - Revision history : 1) 2025.12.31 : Initial release
*******************************************************************/

#include "lcd_words.h"

// Pin Map based on pinmap.txt
// D4-D7: PD0-PD3
// E: PD4
// RS: PD5
#define LCD_PORT PORTD
#define LCD_DDR DDRD
#define LCD_EN_BIT 4
#define LCD_RS_BIT 5

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

const char *const words_table[] PROGMEM = {word0, word1, word2, word3, word4,
                                           word5, word6, word7, word8, word9};

// 전역 변수
char current_word_buffer[17]; // 정답 단어 (원본)
char display_buffer[17];      // 화면 표시 단어 (마스킹)
bool guessed_mask[16];        // 맞춘 글자 체크
int word_length = 0;

static inline void lcd_wait() { _delay_us(40); }

static void lcd_write_nibble(uint8_t rs, uint8_t data) {
  if (rs)
    LCD_PORT |= (1 << LCD_RS_BIT);
  else
    LCD_PORT &= ~(1 << LCD_RS_BIT);

  LCD_PORT |= (1 << LCD_EN_BIT); // E High
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
  // 핀 방향 설정
  LCD_DDR |= (1 << LCD_EN_BIT); // E
  LCD_DDR |= (1 << LCD_RS_BIT); // RS
  LCD_DDR |= 0x0F;              // D4~D7

  _delay_ms(20);
  lcd_write_nibble(0, 3); // 8비트 모드
  lcd_wait();
  lcd_write_nibble(0, 2); // 4비트 모드 설정
  lcd_wait();
  lcd_write_byte(0, 0b00101000); // 4비트, 2라인, 5x8 폰트
  lcd_wait();
  lcd_write_byte(0, 0b00001100); // 화면 켜기
  lcd_wait();
  lcd_clear();
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

void lcd_clearline(bool line) {
  lcd_gotoxy(0, line);
  lcd_puts("                ");
  lcd_gotoxy(0, line);
}

// --- Hangman Logic ---

static void update_word_display() {
  lcd_gotoxy(0, 0); // 첫번째 줄

  for (int i = 0; i < word_length; i++) {
    if (guessed_mask[i]) {
      lcd_write_byte(1, current_word_buffer[i]);
    } else {
      lcd_write_byte(1, '_');
    }
  }
}

void lcd_newword(int index) {
  // 상태 초기화
  for (int i = 0; i < 16; i++)
    guessed_mask[i] = false;

  // 단어 로드
  strcpy_P(current_word_buffer, (char *)pgm_read_word(&(words_table[index])));
  word_length = strlen(current_word_buffer);

  lcd_clear();
  update_word_display();
}

bool lcd_inferword(char c) {
  bool found = false;
  for (int i = 0; i < word_length; i++) {
    if (current_word_buffer[i] == c ||
        current_word_buffer[i] == (c - 32)) { // 대소문자 무시용
      if (!guessed_mask[i]) {
        guessed_mask[i] = true;
        found = true;
      }
    }
  }

  if (found) {
    update_word_display();
  }
  return found;
}

bool lcd_is_wordcomplete(void) {
  for (int i = 0; i < word_length; i++) {
    if (!guessed_mask[i])
      return false;
  }
  return true;
}
