#include "oled_alphabet.h"
#include "bitmap_alphabet.h"
#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

// Pin Assignments based on Pinmap.txt
// PA0 > OLED CS
// PA1 > OLED DC
// PB7 > SCK
// PB6 > DO (MOSI)

#define OLED_CS_DDR DDRA
#define OLED_CS_PORT PORTA
#define OLED_CS_BIT 0

#define OLED_DC_DDR DDRA
#define OLED_DC_PORT PORTA
#define OLED_DC_BIT 1

// 내부 함수: SPI 데이터 전송 (Bit-Banging)
static void SpiTx(uint8_t data) {
  USIDR = data;
  for (uint8_t i = 0; i < 8; i++) {
    // USI 제어 레지스터 설정 (클럭 토글)
    USICR = (USICR & 0xC0) | 0x11;
    USICR = (USICR & 0xC0) | 0x13;
  }
}

static void sh1106_set_location(uint8_t page, uint8_t column) {
  OLED_DC_PORT &= ~(1 << OLED_DC_BIT);  // 명령 모드
  SpiTx(0xb0 | (page & 0x0f));          // 페이지 주소
  SpiTx(0x00 | (column & 0x0f));        // 하위 컬럼 주소
  SpiTx(0x10 | ((column >> 4) & 0x0f)); // 상위 컬럼 주소
}

static inline void sh1106_set_startaddr(uint8_t pos_y, uint8_t delay) {
  SpiTx((1 << 6) | (pos_y & 0x3F));
  _delay_ms(delay);
}

static void oled_scroll(uint8_t scroll_count, uint8_t delay) {
  for (uint8_t i = 0; i < scroll_count; i++) {
    for (uint8_t j = 0; j < 64; j++) {
      sh1106_set_startaddr(j, delay);
    }
  }
}

// public func

void oled_init(void) {
  // 핀 방향 설정
  OLED_CS_DDR |= (1 << OLED_CS_BIT);
  OLED_DC_DDR |= (1 << OLED_DC_BIT);
  SCK_DDR |= (1 << SCK_BIT);
  DO_DDR |= (1 << DO_BIT);

  OLED_CS_PORT |= (1 << OLED_CS_BIT); // CS High (대기)

  // 초기화 시퀀스
  OLED_CS_PORT &= ~(1 << OLED_CS_BIT); // CS Low (시작)

  // 초기화 명령어
  const uint8_t init_commands[] = {
      0xae, // 화면 끄기
      0x00, 0x10, 0xa8, 0x3f, 0xd3, 0x00, 0xd5, 0x50,
      0xd9, 0x22, 0xda, 0x12, 0xdb, 0x35, 0xa4, 0xa6,
      0xa1, // 세그먼트 리맵
      0xc8, // 스캔 방향
      0xaf  // 화면 켜기
  };

  OLED_DC_PORT &= ~(1 << OLED_DC_BIT); // 명령 모드
  for (uint8_t i = 0; i < sizeof(init_commands); i++) {
    SpiTx(init_commands[i]);
  }

  OLED_CS_PORT |= (1 << OLED_CS_BIT); // CS High (종료)
}

void oled_clear(void) {
  OLED_CS_PORT &= ~(1 << OLED_CS_BIT); // CS Low
  for (uint8_t page = 0; page < 8; page++) {
    sh1106_set_location(page, 0);

    OLED_DC_PORT |= (1 << OLED_DC_BIT); // 데이터 모드
    for (uint8_t count = 0; count < 132; count++) {
      SpiTx(0x00);
    }
  }
  OLED_CS_PORT |= (1 << OLED_CS_BIT); // CS High
}

// 문자 출력 (A-Z, 높이 24px)
void oled_putc(char c, uint8_t page, uint8_t column) {
  // 소문자 -> 대문자 변환
  if (c >= 'a' && c <= 'z')
    c -= 32;

  // 범위 체크 (A-Z)
  if (c < 'A' || c > 'Z')
    return;

  uint8_t index = c - 'A';

  const char *font_ptr = alphabet_addr[index];
  uint8_t width = alphabet_width[index];

  if (column + width >= 128)
    return;

  OLED_CS_PORT &= ~(1 << OLED_CS_BIT); // CS Low

  for (unsigned y = 0; y < 2; y++) {
    sh1106_set_location(page + y, column);
    OLED_DC_PORT |= (1 << OLED_DC_BIT); // Data Mode

    const char *p = font_ptr + (y * width);

    for (unsigned x = 0; x < width; x++) {
      SpiTx(pgm_read_byte(p + x));
    }
  }

  OLED_CS_PORT |= (1 << OLED_CS_BIT); // CS High
}

// 내부 헬퍼: 4배 확대된 문자를 GDDRAM에 렌더링
static void render_scaled_char_to_ram(char c) {
  if (c >= 'a' && c <= 'z')
    c -= 32;
  if (c < 'A' || c > 'Z')
    return;

  uint8_t index = c - 'A';
  const char *font_ptr = alphabet_addr[index];
  uint8_t width = alphabet_width[index];

  // 요청된 중앙 정렬 X 위치 계산
  // X = 67 - (width * 2)
  uint8_t x_pos = 67 - (width << 1);

  OLED_CS_PORT &= ~(1 << OLED_CS_BIT); // SPI 시작

  // 8개의 목적지 페이지 (0-7)로 64px 높이를 채움
  for (uint8_t dest_page = 0; dest_page < 8; dest_page++) {
    sh1106_set_location(dest_page, x_pos);
    OLED_DC_PORT |= (1 << OLED_DC_BIT); // 데이터 모드

    // 소스 매핑: 2 소스 페이지 -> 8 목적지 페이지 (1 소스 페이지 -> 4 목적지
    // 페이지) Dest 0-3 -> Src 0, Dest 4-7 -> Src 1
    uint8_t src_page_idx = dest_page / 4;

    // 각 목적지 페이지는 소스의 2비트를 커버함
    // 그룹 0: Dest%4=0 -> 비트 0,1
    // 그룹 1: Dest%4=1 -> 비트 2,3
    // 그룹 2: Dest%4=2 -> 비트 4,5
    // 그룹 3: Dest%4=3 -> 비트 6,7
    uint8_t src_bit_group = dest_page % 4;
    uint8_t bit_a_idx = src_bit_group * 2;
    uint8_t bit_b_idx = src_bit_group * 2 + 1;

    const char *p = font_ptr + (src_page_idx * width);

    for (uint8_t x = 0; x < width; x++) {
      uint8_t src_byte = pgm_read_byte(p + x);
      uint8_t out_byte = 0;

      // 비트 A 확대 -> 하위 니블 (0x0F)
      if (src_byte & (1 << bit_a_idx))
        out_byte |= 0x0F;

      // 비트 B 확대 -> 상위 니블 (0xF0)
      if (src_byte & (1 << bit_b_idx))
        out_byte |= 0xF0;

      // 4배 너비 확대
      SpiTx(out_byte);
      SpiTx(out_byte);
      SpiTx(out_byte);
      SpiTx(out_byte);
    }
  }
  OLED_CS_PORT |= (1 << OLED_CS_BIT); // SPI 종료
}

// 스크롤 효과와 함께 4배 확대 출력
void oled_putc_scaled(char c) {
  // 1. 스크롤 루프 (0 ~ 64 라인)
  // 시작 라인을 회전시켜 슬롯머신 효과 시뮬레이션
  for (uint8_t offset = 0; offset < 64; offset += 2) { // 속도를 위해 2씩 증가
    sh1106_set_startaddr(offset, 0);
    _delay_ms(10); // 속도 조절

    // 2. 스크롤 중간에 컨텐츠 업데이트
    // 화면이 절반 정도 이동했을 때 (offset ~32), GDDRAM 내용을 교체
    if (offset == 32) {
      oled_clear();                 // 이전 문자 잔상 제거
      render_scaled_char_to_ram(c); // 새 문자 그리기
    }
  }

  // 3. 뷰를 정상으로 리셋
  sh1106_set_startaddr(0, 0);
}
