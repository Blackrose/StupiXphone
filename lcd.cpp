#include "lcd.h"
#include "font.h"

int scrollPosition = -10;

void lcd_init(void)
{
  pinMode(PIN_SCE,   OUTPUT);
  pinMode(PIN_RESET, OUTPUT);
  pinMode(PIN_DC,    OUTPUT);
  pinMode(PIN_SDIN,  OUTPUT);
  pinMode(PIN_SCLK,  OUTPUT);
 
  digitalWrite(PIN_RESET, LOW);
  digitalWrite(PIN_RESET, HIGH);
 
  LcdWrite(LCD_CMD, 0x21);  // LCD Extended Commands.
  LcdWrite(LCD_CMD, 0xBf);  // Set LCD Vop (Contrast). //B1
  LcdWrite(LCD_CMD, 0x04);  // Set Temp coefficent. //0x04
  LcdWrite(LCD_CMD, 0x13);  // LCD bias mode 1:48. //0x13
  LcdWrite(LCD_CMD, 0x0c);  // LCD in normal mode. 0x0d for inverse
  LcdWrite(LCD_C, 0x20);
  LcdWrite(LCD_C, 0x0C);
}
 
 
void LcdWrite(byte dc, byte data)
{
  digitalWrite(PIN_DC, dc);
  digitalWrite(PIN_SCE, LOW);
  shiftOut(PIN_SDIN, PIN_SCLK, MSBFIRST, data);
  digitalWrite(PIN_SCE, HIGH);
}


void lcd_char(unsigned int type)
{
    byte *p;
    if(type == SIGNAL_WIDGET){
        p = signal; 
    }else if(type == BATERY_WIDGET)
        p = batery;

    LcdWrite(LCD_D, 0x0);
    
    for(int i = 0; i < 16; i++){
        
        LcdWrite(LCD_D, p[i]);
    }
    
    LcdWrite(LCD_D, 0x0);
}
 
void lcd_clear(void)
{
    for (int index = 0; index < LCD_X * LCD_Y / 8; index++){
        LcdWrite(LCD_D, 0x00);
    }
}

void lcd_clear_row(unsigned int row)
{
    int index = 0;
#if 0
    for(; index < LCD_X; index++){
        lcd_set_xy(index, row);
        LcdWrite(LCD_D, 0x00);
    }
#endif
}

void lcd_string(char *characters)
{
    while (*characters){
        if(*characters == '\r' || *characters == '\n')
            LcdCharacter(' ');
        else
            LcdCharacter(*characters);

        characters++;
    }
}
 
 
/**
 * gotoXY routine to position cursor
 * x - range: 0 to 84
 * y - range: 0 to 5
 */
void lcd_set_xy(int x, int y)
{
    LcdWrite( 0, 0x80 | x);  // Column.
    LcdWrite( 0, 0x40 | y);  // Row.
}


void LcdCharacter(char character)
{
    LcdWrite(LCD_D, 0x00);
    for (int index = 0; index < 5; index++){
        LcdWrite(LCD_D, ASCII[character - 0x20][index]);
    }
    LcdWrite(LCD_D, 0x00);
}


void drawBox(void)
{
    int j;

    // top
    for(j = 0; j < 84; j++){
        lcd_set_xy(j, 0);
        LcdWrite(1, 0x01);
    }
 
    //Bottom
    for(j = 0; j < 84; j++){
        lcd_set_xy(j, 5);
        LcdWrite(1, 0x80);
    }

    // Right
    for(j = 0; j < 6; j++){
        lcd_set_xy(83, j);
        LcdWrite(1, 0xff);
    }
 
    // Left
    for(j = 0; j < 6; j++){
        lcd_set_xy(0, j);
        LcdWrite(1, 0xff);
    }
}


void Scroll(String message)
{
    for (int i = scrollPosition; i < scrollPosition + 11; i++)
    {
        if ((i >= message.length()) || (i < 0)){
            LcdCharacter(' ');
        }else{
            LcdCharacter(message.charAt(i));
        }
    }
  
    scrollPosition++;
    if ((scrollPosition >= message.length()) && (scrollPosition > 0)){
        scrollPosition = -10;
    }
}

void display_string(int row, int column, char* content)
{
    lcd_set_xy(column, row);
    lcd_string(content);
}
