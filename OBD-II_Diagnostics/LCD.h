/******************************************************************************
*
* Module: LCD - 16x2 I2C LCD Driver (PCF8574 I/O Expander)
*
* File Name: LCD.h
*
* Description: Header file for 16x2 LCD with I2C interface for TM4C123GH6PM
*
* Target: TM4C123GH6PM (Tiva C LaunchPad)
*
*******************************************************************************/

#ifndef LCD_H_
#define LCD_H_

#include <stdint.h>
#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                   *
 *******************************************************************************/

/* LCD I2C Address (default for most PCF8574 modules) */
#define LCD_I2C_ADDRESS         0x27    /* Common address, can also be 0x3F */

/* LCD Dimensions */
#define LCD_COLS                16
#define LCD_ROWS                2

/* LCD Commands */
#define LCD_CMD_CLEAR_DISPLAY   0x01
#define LCD_CMD_RETURN_HOME     0x02
#define LCD_CMD_ENTRY_MODE      0x04
#define LCD_CMD_DISPLAY_CTRL    0x08
#define LCD_CMD_CURSOR_SHIFT    0x10
#define LCD_CMD_FUNCTION_SET    0x20
#define LCD_CMD_SET_CGRAM_ADDR  0x40
#define LCD_CMD_SET_DDRAM_ADDR  0x80

/* Entry Mode Flags */
#define LCD_ENTRY_RIGHT         0x00
#define LCD_ENTRY_LEFT          0x02
#define LCD_ENTRY_SHIFT_INC     0x01
#define LCD_ENTRY_SHIFT_DEC     0x00

/* Display Control Flags */
#define LCD_DISPLAY_ON          0x04
#define LCD_DISPLAY_OFF         0x00
#define LCD_CURSOR_ON           0x02
#define LCD_CURSOR_OFF          0x00
#define LCD_BLINK_ON            0x01
#define LCD_BLINK_OFF           0x00

/* Cursor/Display Shift Flags */
#define LCD_DISPLAY_MOVE        0x08
#define LCD_CURSOR_MOVE         0x00
#define LCD_MOVE_RIGHT          0x04
#define LCD_MOVE_LEFT           0x00

/* Function Set Flags */
#define LCD_8BIT_MODE           0x10
#define LCD_4BIT_MODE           0x00
#define LCD_2LINE               0x08
#define LCD_1LINE               0x00
#define LCD_5x10_DOTS           0x04
#define LCD_5x8_DOTS            0x00

/* PCF8574 Pin Mapping for LCD */
#define LCD_RS                  (1 << 0)    /* Register Select */
#define LCD_RW                  (1 << 1)    /* Read/Write */
#define LCD_EN                  (1 << 2)    /* Enable */
#define LCD_BACKLIGHT           (1 << 3)    /* Backlight control */
#define LCD_D4                  (1 << 4)    /* Data bit 4 */
#define LCD_D5                  (1 << 5)    /* Data bit 5 */
#define LCD_D6                  (1 << 6)    /* Data bit 6 */
#define LCD_D7                  (1 << 7)    /* Data bit 7 */

/* Row Addresses */
#define LCD_ROW0_ADDR           0x00
#define LCD_ROW1_ADDR           0x40

/*******************************************************************************
 *                              Function Prototypes                             *
 *******************************************************************************/

/* Initialization Functions */
void LCD_Init(void);
void LCD_Init_I2C(void);

/* Basic LCD Operations */
void LCD_SendCommand(uint8 cmd);
void LCD_SendData(uint8 data);
void LCD_WriteChar(uint8 character);
void LCD_WriteString(const char* str);

/* Display Control Functions */
void LCD_ClearDisplay(void);
void LCD_ReturnHome(void);
void LCD_SetCursor(uint8 row, uint8 col);
void LCD_MoveCursorRight(void);
void LCD_MoveCursorLeft(void);

/* Display Properties */
void LCD_DisplayOn(void);
void LCD_DisplayOff(void);
void LCD_BacklightOn(void);
void LCD_BacklightOff(void);
void LCD_CursorOn(void);
void LCD_CursorOff(void);
void LCD_BlinkOn(void);
void LCD_BlinkOff(void);

/* Custom Character Functions */
void LCD_CreateCustomChar(uint8 location, const uint8* pattern);
void LCD_WriteCustomChar(uint8 location);

/* Utility Functions */
void LCD_WriteNumber(sint32 number);
void LCD_WriteFloat(float32 number, uint8 precision);
void LCD_WriteHex(uint32 number);
void LCD_Printf(uint8 row, uint8 col, const char* str);
void LCD_ClearRow(uint8 row);

/* Scroll Functions */
void LCD_ScrollDisplayLeft(void);
void LCD_ScrollDisplayRight(void);

#endif /* LCD_H_ */
