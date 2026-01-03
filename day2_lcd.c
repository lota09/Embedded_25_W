/*******************************************************************
  - Project          : 2025 Embedded SW
  - File name        : day2_lcd.c
  - Description      : LCD 8bit
  - Owner            : Seokmin Kang
  - Revision history : 1) 2025.12.24 : Initial release
*******************************************************************/

	#define F_CPU 8000000

	#include <avr/io.h>
	#include <util/delay.h>

	#define LCD_EN_DDR DDRD
	#define LCD_EN_PORT PORTD
	#define LCD_EN_BIT 4

	#define LCD_RS_DDR DDRD
	#define LCD_RS_PORT PORTD
	#define LCD_RS_BIT 5

	#define LCD_DB_DDR DDRB
	#define LCD_DB_PORT PORTB
	#define LCD_DB_BIT 0

	static void lcd_write_byte(uint8_t rs, uint8_t data)
	{
		if(rs) LCD_RS_PORT |= (1 << LCD_RS_BIT);
		else   LCD_RS_PORT &= ~(1 << LCD_RS_BIT);              // RS Pin 설정
		LCD_EN_PORT |= (1 << LCD_EN_BIT);  // E Pin 에 1 출력
		LCD_DB_PORT = data;                // DB7~DB0 에 data 값 출력
		LCD_EN_PORT &= ~(1 << LCD_EN_BIT); // E Pin 에 0 출력
	}

	static inline void lcd_wait()
	{
	  _delay_us(40);
	}

	static void lcd_init()
	{
	  LCD_EN_DDR |= (1 << LCD_EN_BIT); // E Pin의 방향을 Output으로 설정
	  LCD_RS_DDR |= (1 << LCD_RS_BIT);
	  LCD_DB_DDR = 0xFF; // DB7~DB0 Pin의 방향을 Output으로 설정

	  _delay_ms(20);                 // Power-on Reset이 실행되기 기다림.
	  lcd_write_byte(0, 0b00111000); // 8-bit mode, 2 lines, 5x8 font
	  lcd_wait();
	  lcd_write_byte(0, 0b00001100); // Display On, Cursor Off, Blinking Off
	  lcd_wait();
	}

	static void lcd_puts(char *str)
	{
	  uint8_t i;
	  lcd_write_byte(0, 0x80); // move to 1st line
	  lcd_wait();
	  for (i = 0; i < 16; i++)
	  {
		if (str[i] == '\0')
		  return;
		lcd_write_byte(1, str[i]);
		lcd_wait();
	  }
	  lcd_write_byte(0, 0x80 + 0x40); // move to 2nd line
	  lcd_wait();
	  for (i = 16; i < 32; i++)
	  {
		if (str[i] == '\0')
		  return;
		lcd_write_byte(1, str[i]);
		lcd_wait();
	  }
	}

	int main(void)
	{
	  uint8_t i = 0;
	  char message[] = "00 seconds";
	  lcd_init();
	  lcd_puts("0123456789ABCDEF!@#$%^&*()abcdef");
	  _delay_ms(2000);
	  lcd_write_byte(0, 0x01); // display clear
	  _delay_ms(2);
	  while (1)
	  {
		message[0] = i / 10 +'0';
		message[1] = i % 10 +'0';
		lcd_puts(message);
		if (++i >= 100)
		  i = 0;
		_delay_ms(1000);
	  }
	}
