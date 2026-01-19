#ifndef __STM_NEOPixel_H
#define __STM_NEOPixel_H

#include "main.h"
#include <string.h>

extern SPI_HandleTypeDef hspi1;
#endif /* __STM_NEOPixel_H */

#define NUM_LED 24
#define RESET_PULSE 16 // 50us ÷ 0.444us ÷ 8

static uint8_t spi_bits[NUM_LED * 3 * 3];
static uint8_t reset_bits[RESET_PULSE] = {0}; // 모두 0으로 초기화

static void set_spi_bits(uint8_t *buf, uint8_t val) {
  uint32_t pulse = 0;
  for (unsigned i = 0; i < 8; i++) {
    uint8_t bit = (val >> (7 - i)) & 1;
    pulse = (pulse << 3) | (bit == 0 ? 0b100 : 0b110);
  }
  buf[0] = (pulse >> 16) & 0xff;
  buf[1] = (pulse >> 8) & 0xff;
  buf[2] = (pulse >> 0) & 0xff;
}

static void clear_neopixel() {
  for (unsigned i = 0; i < NUM_LED; i++) {
    set_spi_bits(spi_bits + i * 9 + 0, 0); // Green  0
    set_spi_bits(spi_bits + i * 9 + 3, 0); // Blue  0
    set_spi_bits(spi_bits + i * 9 + 6, 0); // Red  0
  }
}

static void init_neopixel() {
  clear_neopixel();
  HAL_SPI_Transmit(&hspi1, reset_bits, sizeof(reset_bits), 1);
}

static void set_pixel(uint8_t idx, uint8_t r, uint8_t g, uint8_t b) {
  set_spi_bits(spi_bits + idx * 9 + 0, g);
  set_spi_bits(spi_bits + idx * 9 + 3, r);
  set_spi_bits(spi_bits + idx * 9 + 6, b);
}

static void push_pixels() {
  HAL_SPI_Transmit(&hspi1, spi_bits, sizeof(spi_bits), 2);
  HAL_SPI_Transmit(&hspi1, reset_bits, sizeof(reset_bits), 2);
}

static void set_pixel_array(uint8_t buf[][3], int offset) {
  int start_idx = offset % NUM_LED;
  if (start_idx < 0)
    start_idx += NUM_LED;

  for (unsigned i = 0; i < NUM_LED; i++) {
    set_pixel(i, buf[start_idx][0], buf[start_idx][1], buf[start_idx][2]);
    start_idx++;
    if (start_idx >= NUM_LED)
      start_idx = 0;
  }
}

static void make_rainbow(uint8_t pixels_rainbow[][3], size_t size) {
  for (int i = 0; i < size; i++) {
    uint8_t wheel_pos = i * 256 / size;
    uint8_t r, g, b;

    if (wheel_pos < 85) {
      r = 255 - wheel_pos * 3;
      g = wheel_pos * 3;
      b = 0;
    } else if (wheel_pos < 170) {
      wheel_pos -= 85;
      r = 0;
      g = 255 - wheel_pos * 3;
      b = wheel_pos * 3;
    } else {
      wheel_pos -= 170;
      r = wheel_pos * 3;
      g = 0;
      b = 255 - wheel_pos * 3;
    }

    // set_pixel_array expects RGB format
    pixels_rainbow[i][0] = r;
    pixels_rainbow[i][1] = g;
    pixels_rainbow[i][2] = b;
  }
}

static void left_rotate_pixels(uint8_t n) {
  size_t shift_bytes = n * 9;
  size_t total_bytes = NUM_LED * 9;

  if (shift_bytes >= total_bytes)
    return;

  uint8_t temp[shift_bytes];
  memcpy(temp, spi_bits, shift_bytes);
  memmove(spi_bits, spi_bits + shift_bytes, total_bytes - shift_bytes);
  memcpy(spi_bits + (total_bytes - shift_bytes), temp, shift_bytes);
}

static void right_rotate_pixels(uint8_t n) {
  size_t shift_bytes = n * 9;
  size_t total_bytes = NUM_LED * 9;

  if (shift_bytes >= total_bytes)
    return;

  uint8_t temp[shift_bytes];
  memcpy(temp, spi_bits + (total_bytes - shift_bytes), shift_bytes);
  memmove(spi_bits + shift_bytes, spi_bits, total_bytes - shift_bytes);
  memcpy(spi_bits, temp, shift_bytes);
}