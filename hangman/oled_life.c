/*******************************************************************
  - Project          : 2025 Embedded SW
  - File name        : oled_life.c
  - Description      : OLED display and life logic for hangman game 
  - Owner            : Seokmin Kang
  - Revision history : 1) 2025.12.31 : Initial release 
*******************************************************************/
#include "oled_life.h"

// ... (SPI functions remain same)

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
    life = 5;

    const uint8_t init_commands[] = {
        0xae, 0x00, 0x10, // Set Page & Column Addressing Mode
        0xa8, 0x3f, 0xd3, 0x00, 0xd5, 0x50, 0xd9, 0x22, // 
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

// 16 pixels height (2 pages) bitmap drawing
// Assumes bitmap data is stored by columns: Page 0/Col 0, P0/C1... then Page 1/Col 0...
void sh1106_draw_bitmap(const uint8_t *bitmap, uint8_t width, uint8_t page, uint8_t column)
{
    OLED_CS_PORT &= ~(1 << OLED_CS_BIT); // CS -> 0
    // We draw 2 pages
    for (uint8_t y = 0; y < 2; y++)
    {
        sh1106_set_location(page + y, column);
        OLED_DC_PORT |= (1 << OLED_DC_BIT); // DC -> 1
        for (uint8_t x = 0; x < width; x++)
        {
             // Calculate offset: Row y * width + x
             // bitmap is in PROGMEM
             SpiTx(pgm_read_byte(&bitmap[y * width + x]));
        }
    }
    OLED_CS_PORT |= (1 << OLED_CS_BIT); // CS -> 1
}

void display_lives() {
    if (lives > 5) lives = 5;
    
    // Clear the heart area (Assume Page 2-3, full width)
    // Simply redrawing hearts is fine, or we can clear specific area if needed.
    // Here we just draw 'lives' number of hearts.
    // To handle clearing lost hearts, we might want to clear the line first or draw empty hearts.
    // For this simple test, we just draw the hearts.
    
    uint8_t start_col = 10;
    uint8_t gap = 4;
    
    for(int i=0; i<lives; i++) {
        // Fetch heart bitmap address from table (index 0)
        const uint8_t* bmp = (const uint8_t*)pgm_read_word(&oled_bitmap_addr[0]); 
        uint8_t w = pgm_read_byte(&oled_bitmap_width[0]);
        sh1106_draw_bitmap(bmp, w, 2, start_col + i * (w + gap));
    }
}

void lose_life()
{
    life--;
    display_lives();
}

bool is_lost()
{
    if(life > 0)
        return false;
    else
        return true;
}

int main(void)
{
    //// USCK, DO 핀의 DDR 설정
    DDRB |= (1 << PB7) | (1 << PB6); // PB7: SCK, PB6: MOSI

    sh1106_init();
    sh1106_clear();
    
    // Display 5 hearts
    display_lives(5);
    
    while (1)
    {
        // Blink logic removed to avoid screen flicker for now
        // Just keeping the display on
    }
}