/*******************************************************************
- Project          : 2025 Embedded SW
- File name        : day6_usart_rx.c
- Description      : USART RX example - serial speed 38400, crystal oscillator 16MHz
- Owner            : Seokmin Kang
- Revision history : 1) 2025.12.24 : Initial release
*******************************************************************/

#define F_CPU 16000000
#define BAUD 38400

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/setbaud.h>

static void uart_init(void)
{
    UBRRH = UBRR_VALUE >> 8;
    UBRRL = UBRR_VALUE;
    
    UCSRB = (1 << TXEN) | (1 << RXEN); // UCSRB의 TXEN과 RXEN를 활성화
    UCSRC = 0b00000110; // UCSRC를 Async, 8 data, 1 stop, no parity로 설정.

    // UMSEL , UMP : p140
    // UCPOL : p141
    // [7:6]Asynchronous, [5:4]no parity, [3]1 stop, [2:1]5+3 data, [0]UCPOL : only used in synchronous mode
    /*
    UCSRC = (0 << UMSEL1) | \
            (0 << UMSEL0) | \
            (0 << UPM1) | \
            (0 << UPM0) | \
            (0 << USBS) | \
            (3 << UCSZ0) | \
            (0 << UCPOL);    
    */
}

static void uart_tx(uint8_t ch)
{
    // UCSRA의 UDRE bit가 1이 될 때까지 기다림.
    while (!(UCSRA & (1 << UDRE)))
        ;
    
    UDR = ch; // UDR에 ch를 write.
}

static void uart_tx_str(char *str)
{
    while (*str)
    {
        uart_tx(*str++);
    }
}

int main(void)
{
    uint8_t i = 0;
    uart_init();
    uart_tx_str("Hello World UART Serial\n");
    // Send '0' to '9' every second
    while (1)
    {
        uart_tx('0' + i);
        uart_tx('\n');
        _delay_ms(1000);
        if (++i > 9)
            i = 0;
    }
}