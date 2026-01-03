/*******************************************************************
  - Project          : 2025 Embedded SW
  - File name        : day4_oled.c
  - Description      : OLED
  - Owner            : Seokmin Kang
  - Revision history : 1) 2025.12.30 : Initial release
*******************************************************************/

#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>

#define OLED_CS_DDR DDRB
#define OLED_CS_PORT PORTB
#define OLED_CS_BIT 0

#define OLED_DC_DDR DDRB
#define OLED_DC_PORT PORTB
#define OLED_DC_BIT 1

static void SpiTx(uint8_t data)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        uint8_t bit = (data >> (7 - i)) & 1; // MSB부터 차례로 1 bit를 추출
        DO_PORT &= ~(1 << DO_BIT);           // PORTB 레지스터 Bit 6에 0을 write
        DO_PORT |= (bit << DO_BIT);          // PORTB 레지스터 Bit 6에 bit 값을 write
        // → DIN 핀에 bit 값을 출력
        SCK_PORT |= (1 << SCK_BIT);  // CLK 핀에 1 출력
        SCK_PORT &= ~(1 << SCK_BIT); // CLK 핀에 0 출력
    }
}

void sh1106_init(void)
{
    const uint8_t init_commands[] = {
        0xae, 0x00, 0x10,
        0xa8, 0x3f, 0xd3, 0x00, 0xd5, 0x50, 0xd9, 0x22,
        0xda, 0x12, 0xdb, 0x35, 0xa4, 0xa6, 0xa1, 0xc8, 0xaf};
    // CS, DC 핀의 DDR 설정.
    OLED_CS_DDR |= (1 << OLED_CS_BIT);
    OLED_DC_DDR |= (1 << OLED_DC_BIT);

    // CS, DC 핀 초기 상태 0, 0 설정.
    OLED_CS_PORT &= ~(1 << OLED_CS_BIT);
    OLED_DC_PORT &= ~(1 << OLED_DC_BIT);

    for (uint8_t i = 0; i < sizeof(init_commands); i++)
    {
        SpiTx(init_commands[i]);
    }

    OLED_CS_PORT |= (1 << OLED_CS_BIT); // CS -> 1
}

void sh1106_set_location(uint8_t page, uint8_t column)
{
    OLED_DC_PORT &= ~(1 << OLED_DC_BIT);  // DC -> 0
    SpiTx(0xb0 | (page & 0x0f));          // page 주소 설정
    SpiTx(0x00 | (column & 0x0f));        // column 주소 설정
    SpiTx(0x10 | ((column >> 4) & 0x0f)); // column 주소 설정
}

void sh1106_clear(void)
{
    OLED_CS_PORT &= ~(1 << OLED_CS_BIT); // CS -> 0
    for (uint8_t page = 0; page < 8; page++)
    {
        sh1106_set_location(page, 0);

        OLED_DC_PORT |= (1 << OLED_DC_BIT); // DC -> 1
        for (uint8_t count = 0; count < 132; count++)
        {
            SpiTx(0);
        }
    }
    OLED_CS_PORT |= (1 << OLED_CS_BIT); // CS -> 1
}

void sh1106_testpattern(void)
{
    OLED_CS_PORT &= ~(1 << OLED_CS_BIT); // CS -> 0
    for (uint8_t page = 0; page < 8; page++)
    {
        sh1106_set_location(page, 34);
        OLED_DC_PORT |= (1 << OLED_DC_BIT); // DC -> 1
        for (uint8_t count = 0; count < 64; count++)
        {
            SpiTx(0xf0);
        }
    }
    OLED_CS_PORT |= (1 << OLED_CS_BIT); // CS -> 1
}

void sh1106_border(void)
{
    OLED_CS_PORT &= ~(1 << OLED_CS_BIT); // CS -> 0

    // Top Border
    sh1106_set_location(0, 0);
    OLED_DC_PORT |= (1 << OLED_DC_BIT); // DC -> 1
    for (uint8_t column = 0; column < 132; column++)
    {
        SpiTx(0x01);
    }

    // Bottom Border
    sh1106_set_location(7, 0);
    OLED_DC_PORT |= (1 << OLED_DC_BIT); // DC -> 1
    for (uint8_t column = 0; column < 132; column++)
    {
        SpiTx(0x80);
    }

    for (uint8_t page = 0; page < 8; page++)
    {
        // Left Border
        sh1106_set_location(page, 2);
        OLED_DC_PORT |= (1 << OLED_DC_BIT); // DC -> 1
        SpiTx(0xff);

        // Right Border
        sh1106_set_location(page, 129);
        OLED_DC_PORT |= (1 << OLED_DC_BIT); // DC -> 1
        SpiTx(0xff);
    }
    OLED_CS_PORT |= (1 << OLED_CS_BIT); // CS -> 1
}

int main(void)
{
    //// USCK, DO 핀의 DDR 설정
    DDRB |= (1 << PB7) | (1 << PB6); // PB7: SCK, PB6: MOSI

    // SPI 설정: Master, F_CPU/16
    // SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);

    sh1106_init();
    
    while (1)
    {
        sh1106_clear();
        _delay_ms(100);

        sh1106_testpattern();
        _delay_ms(1000);

        sh1106_border();
        _delay_ms(1000);
    }
}