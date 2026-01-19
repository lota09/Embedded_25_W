/*******************************************************************
  - Project          : 2025 Embedded SW
  - File name        : 7seg_life.h
  - Description      : life counter for hangman game
  - Owner            : Seokmin Kang
  - Revision history : 1) 2025.12.31 : Initial release
*******************************************************************/

#ifndef SEVEN_SEG_LIFE_H
#define SEVEN_SEG_LIFE_H

#define F_CPU 8000000

#include <stdbool.h>
#include <stdint.h>

void seg7_init(void);
void seg7_display_lives(void);
void seg7_lose_life(void);
bool seg7_is_lost(void);
void seg7_show_lost(void);
void seg7_show_won(void);

#endif