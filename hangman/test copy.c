#define F_CPU 8000000

#include "7seg_life.h"
#include "lcd_words.h"
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>

// Helper to simulate a game round with predetermined guesses
void play_scenario(const char *word_to_guess, const char *guesses,
                   bool expect_win) {
  lcd_clear();
  char buffer[17];

  // 1. Setup New Game
  seg7_init(); // Reset Life to 5 (and "LIFE" text on left)

  // Find index for the word. For testing, we know H=0, W=1, A=2 ...
  // Let's assume index 0 is "HELLO".
  lcd_newword(0);

  _delay_ms(1000);

  int guess_idx = 0;

  while (!lcd_is_wordcomplete() && !seg7_is_lost()) {
    char c = guesses[guess_idx];
    if (c == '\0')
      break; // No more guesses provided

    guess_idx++;

    bool result = lcd_inferword(c);

    lcd_clearline(1);
    lcd_gotoxy(0, 1);
    if (result) {
      sprintf(buffer, "%c: Correct!", c);
    } else {
      seg7_lose_life();
      sprintf(buffer, "%c: Wrong!", c);
    }
    lcd_puts(buffer);

    _delay_ms(1000);
  }

  lcd_clearline(1);
  lcd_gotoxy(0, 1);

  if (lcd_is_wordcomplete() && !seg7_is_lost()) {
    seg7_show_won();
    lcd_puts("You Got It!");
  } else if (seg7_is_lost()) {
    seg7_show_lost();
    lcd_puts("Maybe Nexttime..");
  } else {
    lcd_puts("Test Error?");
  }
  _delay_ms(3000);
}

int main(void) {
  lcd_init();
  seg7_init();

  lcd_clear();
  lcd_puts("Integrated Test");
  _delay_ms(1000);

  while (1) {
    // Scenario 1: WIN ("HELLO")
    // Guesses: H, E, L, O
    play_scenario("HELLO", "HREVLO", true);

    // Scenario 2: LOSS ("HELLO")
    // Guesses: Z, X, C, V, B, N (6 wrong guesses -> Die)
    play_scenario("HELLO", "ZXCVBN", false);
  }
}
