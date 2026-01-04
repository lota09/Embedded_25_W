/*******************************************************************
- Project          : 2025 Embedded SW
- File name        : 7seg_timer.c
- Description      : 7 Segment Display
- Owner            : Seokmin Kang
- Revision history : 1) 2025.12.24 : Initial release
*******************************************************************/

static void SpiTx(uint8_t data)
{
    USIDR = data;
    for (uint8_t i = 0; i < 8; i++)
    {
        USICR = (USICR & 0xC0) | (1 << USIWM0) | (0 << USICS1) | (0 << USICS0) | (0 << USICLK) | (1 << USITC); // USIWM=01, USICS=00, USICLK=0, USITC=1
        USICR = (USICR & 0xC0) | (1 << USIWM0) | (0 << USICS1) | (0 << USICS0) | (1 << USICLK) | (1 << USITC); // USIWM=01, USICS=00, USICLK=1, USITC=1
    }
}

static void max7219_write(uint8_t addr, uint8_t data)
{
    MAX_CS_PORT &= ~(1 << MAX_CS_BIT); // CS 핀에 0 출력 (전송 시작)
    SpiTx(addr);                       // CLK과 DIN 핀에 8-bit addr 전송
    SpiTx(data);                       // CLK과 DIN 핀에 8-bit data 전송
    MAX_CS_PORT |= (1 << MAX_CS_BIT);  // CS 핀에 1 출력 (전송 종료)
}

static void max7219_init(void)
{
    const uint8_t initData[] = {
        0x09, 0xff, // Decode Mode: Code B for all digits
        0x0a, 0x01, // Intensity: 1 (low brightness)
        0x0b, 0x07, // Scan Limit: Display all 8 digits
        0x0c, 0x01, // Shutdown Register: Normal Operation
        0x0f, 0x00 // Display Test: Off
    };
    
    // USCK, DO, CS 핀의 DDR 설정
    USCK_DDR |= (1 << USCK_BIT);
    DO_DDR |= (1 << DO_BIT);
    MAX_CS_DDR |= (1 << MAX_CS_BIT);

    // CS의 초기값을 1로 설정 (inactive)
    MAX_CS_PORT |= (1 << MAX_CS_BIT);
    
    // initData[]의 값을 addr, data의 짝으로 max7219_write 함수에 보냄.
    for (uint8_t i = 0; i < sizeof(initData); i += 2)
    {
        max7219_write(initData[i], initData[i + 1]);
    }
}

void max7219_writeint(int int_number) // fatal : 2byte int 25565 max value
{
    uint8_t digit = 1
    while (int_number >= 10)
    {
        if(digit > 8) // exceed display limit
            break;

        max7219_write(digit, int_number % 10);
        int_number /= 10 ;
        digit ++;
    }

}

int main(void)
{
    max7219_init();
    while (1)
    {
        for (uint8_t number = 0; number < 10; number++)
        {
            for (uint8_t digit = 0; digit < 8; digit++)
            {
                max7219_write((7 - digit) + 1, (number + digit) % 10);
            }
            _delay_ms(1000);
        }
    }
}