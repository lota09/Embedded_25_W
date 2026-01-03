/*******************************************************************
- Project          : 2025 Embedded SW
- File name        : day5_interrupt.c
- Description      : Rotary Encoder interrupt example
- Owner            : Seokmin Kang
- Revision history : 1) 2025.12.24 : Initial release
*******************************************************************/

#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define USCK_DDR DDRB
#define USCK_PORT PORTB
#define USCK_BIT 7

#define DO_DDR DDRB
#define DO_PORT PORTB
#define DO_BIT 6

#define MAX_CS_DDR DDRB
#define MAX_CS_PORT PORTB
#define MAX_CS_BIT 5

#define ROE_A_DDR DDRA
#define ROE_A_PORT PORTA
#define ROE_A_BIT 1

#define ROE_B_DDR DDRD
#define ROE_B_PORT PORTD
#define ROE_B_BIT 4

static volatile int16_t counter=100;

ISR(PCINT1_vect)
{
    uint8_t a = ; //// A핀의 값을 읽음;
    uint8_t b = ; //// B핀의 값을 읽음;
    //// A와 B 값이 다르면 counter를 증가;
    //// 그렇지 않으면 counter를 감소;
    //// counter가 199보다 커지면 0으로 초기화
    //// counter가 0보다 작아지면 199로 초기화
}

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
        0x0b, 0x01, // Scan Limit: Display only 2 digits
        0x0c, 0x01, // Shutdown Register: Normal Operation
        0x0f, 0x00  // Display Test: Off
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

int main(void)
{
    int16_t prev_counter = 0;
    max7219_init(); // SPI 초기화 (USI의 USCK, DO 핀 DDR 설정)
    ROE_A_DDR &= ~(1 << ROE_A_BIT); // A핀을 입력으로 설정
    ROE_B_DDR &= ~(1 << ROE_B_BIT); // B핀을 입력으로 설정
    ROE_A_PORT |= (1 << ROE_A_BIT); // A핀의 Pull-Up 저항 활성화
    ROE_B_PORT |= (1 << ROE_B_BIT); // B핀의 Pull-Up 저항 활성화
    PCMSK1 |= (1 << PCINT9); // PCMSK1 레지스터에서 PCINT9 활성화
    GIMSK |= (1 << PCIE1); // GIMSK 레지스터에서 PCIE1 활성화
    sei(); // 전체 인터럽트 활성화

    //max7219_init 에서 이미 수행
        //// MAX7219 초기화
        //// MAX7219 하위 2자리만 표시하도록 설정 (scan limit 사용 가능)

    while (1)
    {
        if (counter != prev_counter)
        { //// counter 값이 바뀌었으면
            prev_counter = counter;
            // counter 값을 1/2로 나눠서 2자리로 MAX7219에 표시
            
            max7219_write(0x01, counter%10);         // counter 값을 10으로 나눈 나머지 1자리로 MAX7219에 표시
            max7219_write(0x02, (counter/10)%10);    //
        }
    }
}