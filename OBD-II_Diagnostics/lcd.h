/******************************************************************************
 *
 * Module: LCD (I2C)
 *
 * File Name: lcd.h
 *
 * Description: Header file for 16x2 I2C LCD Driver (PCF8574)
 *
 *******************************************************************************/

#ifndef LCD_H_
#define LCD_H_

#include "std_types.h"

/* Default I2C Address for PCF8574 is usually 0x27 or 0x3F */
#define LCD_SLAVE_ADDRESS  0x27

/* Public Functions */
void LCD_Init(void);
void LCD_SendCommand(uint8 command);
void LCD_SendData(uint8 data);
void LCD_Clear(void);
void LCD_SetCursor(uint8 row, uint8 col);
void LCD_WriteString(const char *str);

#endif /* LCD_H_ */
