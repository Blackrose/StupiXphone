#ifndef LCD_H
#define LCD_H
#include "Arduino.h"

// The pins to use on the arduino
#define PIN_SCE   7
#define PIN_RESET 6
#define PIN_DC    5
#define PIN_SDIN  4
#define PIN_SCLK  3
 
// COnfiguration for the LCD
#define LCD_C     LOW
#define LCD_D     HIGH
#define LCD_CMD   0

// Size of the LCD
#define LCD_X     84
#define LCD_Y     48

enum statusbar_widget{
    SIGNAL_WIDGET = 1,
    BATERY_WIDGET = 2,
};

void lcd_init(void);
void LcdWrite(byte dc, byte data);
void lcd_char(unsigned int);
void lcd_clear(void);
void lcd_string(char *characters);
void lcd_set_xy(int x, int y);
void LcdCharacter(char character);

void lcd_clear_row(unsigned int row);

#endif
