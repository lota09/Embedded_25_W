
/*******************************************************************
  - Project          : 2025 Embedded SW
  - File name        : day7_buzzer.c
  - Description      : Buzzer Tone Generation using Timer1 CTC
  - Owner            : Seokmin Kang
  - Revision history : 2026.01.12 : Implementation
*******************************************************************/

#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>

#define BUZ_DDR DDRB
#define BUZ_BIT 3 // OC1A (PB3)

static const uint16_t scale[] = {
    238, // Do (C7)
    212, // Re (D7)
    189, // Mi (E7)
    178, // Fa (F7)
    158, // Sol (G7)
    141, // La (A7)
    126, // Si (B7)
    118  // Do (C8)
    // 도,레,미,파,솔,라,시,도
};

static void tone(uint8_t octave, uint8_t note)
{
    if (octave == 0 || octave > 7 || note >= 8)
    {
        // 소리를 꺼야 하는 경우
        //// COM1A 값을 Clear on Compare Match로 하여 항상 0을 출력
        // TCCR1A: COM1A1=1, COM1A0=0 (Clear OC1A on Compare Match) 
        // But to stop sound clearly, we usually enforce Low or disable Timer output.
        // The comment says "Clear on Compare Match which outputs 0".
        // If we just stop the timer, that works too, but let's follow bits.
        // Actually, disconnection (Normal port operation) is better to turn off.
        // But "Clear on Compare Match" implies setting COM1A1:0 to 10. 
        // Wait, if we are in CTC mode, Clear on Match means it stays Low only if it started Low? 
        // Usually 0 to disconnect.
        // Let's assume standard "Disconnect" (COM1A1=0, COM1A0=0).
        TCCR1A &= ~((1 << COM1A1) | (1 << COM1A0));
        TCCR1B = 0; // Stop timer
        PORTB &= ~(1 << BUZ_BIT); // Ensure Low
    }
    else
    {
        // 소리를 내야 하는 경우
        //// TCNT1을 0으로 초기화
        TCNT1 = 0;
        
        //// WGM1 값을 CTC Mode (TOP=OCR1A)로 설정
        // WGM13:0 = 0100 (CTC, Top=OCR1A) -> WGM12=
        TCCR1A &= !(1 << WGM00);
        TCCR1A |= (1 << WGM01);
        TCCR1B &= !(1 << WGM02);
        
        //// COM1A 값을 Toggle로 설정
        // COM1A1:0 = 01 (Toggle OC1A on Compare Match)
        
        //// Clock source와 Pre-scale 값을 설정하여 카운트 시작
        // CS12:0 = 010 (Prescaler 8)
        
        // TCCR1A = (0<<COM1A1) | (1<<COM1A0) | (0<<WGM11) | (0<<WGM10);
        TCCR1A = (1 << COM1A0); 
        
        // TCCR1B = (0<<ICNC1) | (0<<ICES1) | (0<<WGM13) | (1<<WGM12) | (0<<CS12) | (1<<CS11) | (0<<CS10);
        TCCR1B = (1 << WGM12) | (1 << CS11);

        //// scale 값을 읽어와 octave에 맞게 shift 한 후 OCR1A에 설정
        // Our scale is for Octave 7. 
        // If request is Octave 7, shift 0. 
        // If request is Octave 6, shift 1 (Double duration).
        // Shift = 7 - octave.
        OCR1A = scale[note] << (7 - octave);
    }
}

static void tone(uint8_t octave, uint8_t note) {
if (octave==0 || octave>7 || note>=8) {
    // 소리를 꺼야 하는 경우
    //// COM1A 값을 Clear on Compare Match로 하여 항상 0을 출력
    }
    else{
    // 소리를 내야 하는 경우
    //// TCNT1을 0으로 초기화
    //// WGM1 값을 CTC Mode (TOP=OCR1A)로 설정
    TCCR1A &= !(1 << WGM00);
    TCCR1A |= (1 << WGM01);
    TCCR1B &= !(1 << WGM02);
    //// COM1A 값을 Toggle로 설정
    TCCR1A &= !(1 << COM1A1);
    TCCR1A |= (1 << COM1A0); 
    //// scale 값을 읽어와 octave에 맞게 shift 한 후 OCR1A에 설정
    //// Clock source와 Pre-scale 값을 설정하여 카운트 시작
    }
}

int main(void)
{
    BUZ_DDR |= 1 << BUZ_BIT; // OC1A Pin을 출력으로 설정
    while (1)
    {
        for (uint8_t octave = 3; octave < 6; octave++)
        {
            for (uint8_t note = 0; note < 8; note++)
            {
                tone(octave, note);
                _delay_ms(1000); // 음을 1초 출력
                tone(0, 0);
                _delay_ms(100); // 음 사이에 0.1초 쉼
            }
            tone(0, 0);
            _delay_ms(2000); // 옥타브 사이에 2초 쉼
        }
    }
}