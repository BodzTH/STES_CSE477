/******************************************************************************
 *
 * Module: LCD (I2C)
 *
 * File Name: lcd.c
 *
 * Description: Source file for 16x2 I2C LCD Driver
 * Uses I2C1 and Delay_MS
 *
 *******************************************************************************/

#include "lcd.h"
#include "i2c.h"
#include "delay.h"

/* Internal Bit Masks for PCF8574 */
#define LCD_RS         0x01
#define LCD_RW         0x02
#define LCD_EN         0x04
#define LCD_BACKLIGHT  0x08

static void LCD_WriteNibble(uint8 nibble, uint8 controlBits)
{
    uint8 data = (nibble & 0xF0) | controlBits | LCD_BACKLIGHT;
    
    /* Enable Pulse */
    I2C1_WriteByte(LCD_SLAVE_ADDRESS, data | LCD_EN);
    Delay_MS(1); /* Small delay for Enable pulse */
    I2C1_WriteByte(LCD_SLAVE_ADDRESS, data & ~LCD_EN);
    Delay_MS(1);
}

static void LCD_SendByte(uint8 value, uint8 mode)
{
    uint8 highNibble = value & 0xF0;
    uint8 lowNibble  = (value << 4) & 0xF0;

    LCD_WriteNibble(highNibble, mode);
    LCD_WriteNibble(lowNibble, mode);
}

void LCD_SendCommand(uint8 command)
{
    LCD_SendByte(command, 0); /* RS = 0 for Command */
}

void LCD_SendData(uint8 data)
{
    LCD_SendByte(data, LCD_RS); /* RS = 1 for Data */
}

void LCD_Init(void)
{
    /* 1. Initialize I2C1 */
    I2C1_Init(I2C_BAUDRATE_100K);
    
    /* 2. Wait for LCD to power up */
    Delay_MS(50);

    /* 3. Reset Sequence (Standard HD44780) */
    LCD_WriteNibble(0x30, 0);
    Delay_MS(5);
    LCD_WriteNibble(0x30, 0);
    Delay_MS(1);
    LCD_WriteNibble(0x30, 0);
    Delay_MS(1);
    
    /* 4. Set to 4-bit mode */
    LCD_WriteNibble(0x20, 0);
    Delay_MS(1);

    /* 5. Configuration */
    LCD_SendCommand(0x28); /* Function Set: 4-bit, 2 lines, 5x8 font */
    LCD_SendCommand(0x0C); /* Display ON, Cursor OFF, Blink OFF */
    LCD_SendCommand(0x06); /* Entry Mode: Auto-increment */
    LCD_Clear();
}

void LCD_Clear(void)
{
    LCD_SendCommand(0x01); /* Clear Display */
    Delay_MS(2);           /* This command is slow */
}

void LCD_SetCursor(uint8 row, uint8 col)
{
    uint8 address;
    if(row == 0)
        address = 0x80 + col;
    else
        address = 0xC0 + col;
        
    LCD_SendCommand(address);
}

void LCD_WriteString(const char *str)
{
    while(*str)
    {
        LCD_SendData(*str++);
    }
}
