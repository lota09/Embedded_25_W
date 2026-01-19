#include <stdint.h>
#define F_CPU 8000000

#include "oled_alphabet.h"
#include <avr/io.h>
#include <util/delay.h>

int main(void) {
  // OLED 초기화
  oled_init();
  oled_clear();

  while (1) {
    for (uint8_t i = 'A'; i <= 'Z'; i++) {
      oled_clear();
      oled_putc(i, 3, 50);
      _delay_ms(3000);
    }
  }
}
