/******************************************************************************
 *
 * Module: LCD - 16x2 I2C LCD Driver (PCF8574 I/O Expander)
 *
 * File Name: LCD.c
 *
 * Description: Source file for 16x2 LCD with I2C interface for TM4C123GH6PM
 *
 * Target: TM4C123GH6PM (Tiva C LaunchPad)
 *
 *******************************************************************************/

#include "lcd.h"
#include "delay.h"
#include "i2c.h"
#include <string.h>

/*******************************************************************************
 *                              Private Variables  *
 *******************************************************************************/

static uint8 g_backlightState = LCD_BACKLIGHT; /* Backlight ON by default */
static uint8 g_displayControl = LCD_DISPLAY_ON | LCD_CURSOR_OFF | LCD_BLINK_OFF;

/*******************************************************************************
 *                              Private Functions  *
 *******************************************************************************/

/*
 * Description: Send 4-bit data to LCD via I2C
 */
static void LCD_Write4Bits(uint8 data) {
  uint8 i2cData;

  /* Combine data with backlight state */
  i2cData = data | g_backlightState;

  /* Send data with Enable HIGH */
  I2C_WriteByte(LCD_I2C_ADDRESS, i2cData | LCD_EN);
  Delay_US(1);

  /* Send data with Enable LOW (latch data) */
  I2C_WriteByte(LCD_I2C_ADDRESS, i2cData & ~LCD_EN);
  Delay_US(50);
}

/*
 * Description: Send a byte to LCD in 4-bit mode
 * Parameters:
 *   - data: The byte to send
 *   - mode: 0 for command, LCD_RS for data
 */
static void LCD_SendByte(uint8 data, uint8 mode) {
  uint8 highNibble = (data & 0xF0) | mode;
  uint8 lowNibble = ((data << 4) & 0xF0) | mode;

  /* Send high nibble first, then low nibble */
  LCD_Write4Bits(highNibble);
  LCD_Write4Bits(lowNibble);
}

/*******************************************************************************
 *                              Public Functions  *
 *******************************************************************************/

/*
 * Description: Initialize I2C module for LCD communication
 */
void LCD_Init_I2C(void) {
  /* Initialize I2C0 module */
  /* Initialize I2C1 module (PA6/PA7) */
  I2C_InitModule(I2C_MODULE_1, I2C_SPEED_STANDARD);
}

/*
 * Description: Initialize the LCD display
 */
void LCD_Init(void) {
  /* Initialize I2C communication */
  LCD_Init_I2C();

  /* Wait for LCD power-up (>40ms after VCC rises to 2.7V) */
  Delay_MS(50);

  /* Initialize backlight state */
  I2C_WriteByte(LCD_I2C_ADDRESS, g_backlightState);
  Delay_MS(1000);

  /*
   * LCD Initialization Sequence for 4-bit mode
   * According to HD44780 datasheet
   */

  /* Step 1: Function set (8-bit mode) - first time */
  LCD_Write4Bits(0x30);
  Delay_MS(5); /* Wait >4.1ms */

  /* Step 2: Function set (8-bit mode) - second time */
  LCD_Write4Bits(0x30);
  Delay_US(150); /* Wait >100us */

  /* Step 3: Function set (8-bit mode) - third time */
  LCD_Write4Bits(0x30);
  Delay_US(150);

  /* Step 4: Function set (4-bit mode) */
  LCD_Write4Bits(0x20);
  Delay_US(150);

  /* Now in 4-bit mode, can use LCD_SendCommand */

  /* Function Set: 4-bit mode, 2 lines, 5x8 dots */
  LCD_SendCommand(LCD_CMD_FUNCTION_SET | LCD_4BIT_MODE | LCD_2LINE |
                  LCD_5x8_DOTS);

  /* Display Control: Display ON, Cursor OFF, Blink OFF */
  g_displayControl = LCD_DISPLAY_ON | LCD_CURSOR_OFF | LCD_BLINK_OFF;
  LCD_SendCommand(LCD_CMD_DISPLAY_CTRL | g_displayControl);

  /* Clear Display */
  LCD_ClearDisplay();

  /* Entry Mode Set: Increment cursor, no display shift */
  LCD_SendCommand(LCD_CMD_ENTRY_MODE | LCD_ENTRY_LEFT | LCD_ENTRY_SHIFT_DEC);

  /* Return cursor to home position */
  LCD_ReturnHome();
}

/*
 * Description: Send a command to the LCD
 */
void LCD_SendCommand(uint8 cmd) {
  LCD_SendByte(cmd, 0);

  /* Commands like clear display and return home need longer delay */
  if (cmd == LCD_CMD_CLEAR_DISPLAY || cmd == LCD_CMD_RETURN_HOME) {
    Delay_MS(2);
  }
}

/*
 * Description: Send data (character) to the LCD
 */
void LCD_SendData(uint8 data) { LCD_SendByte(data, LCD_RS); }

/*
 * Description: Write a single character to the LCD
 */
void LCD_WriteChar(uint8 character) { LCD_SendData(character); }

/*
 * Description: Write a string to the LCD
 */
void LCD_WriteString(const char *str) {
  while (*str) {
    LCD_WriteChar(*str++);
  }
}

/*
 * Description: Clear the LCD display
 */
void LCD_ClearDisplay(void) {
  LCD_SendCommand(LCD_CMD_CLEAR_DISPLAY);
  Delay_MS(2);
}

/*
 * Description: Return cursor to home position (0,0)
 */
void LCD_ReturnHome(void) {
  LCD_SendCommand(LCD_CMD_RETURN_HOME);
  Delay_MS(2);
}

/*
 * Description: Set cursor position
 * Parameters:
 *   - row: 0 or 1
 *   - col: 0 to 15
 */
void LCD_SetCursor(uint8 row, uint8 col) {
  uint8 address;

  if (row == 0) {
    address = LCD_ROW0_ADDR + col;
  } else {
    address = LCD_ROW1_ADDR + col;
  }

  LCD_SendCommand(LCD_CMD_SET_DDRAM_ADDR | address);
}

/*
 * Description: Move cursor one position to the right
 */
void LCD_MoveCursorRight(void) {
  LCD_SendCommand(LCD_CMD_CURSOR_SHIFT | LCD_CURSOR_MOVE | LCD_MOVE_RIGHT);
}

/*
 * Description: Move cursor one position to the left
 */
void LCD_MoveCursorLeft(void) {
  LCD_SendCommand(LCD_CMD_CURSOR_SHIFT | LCD_CURSOR_MOVE | LCD_MOVE_LEFT);
}

/*
 * Description: Turn on the LCD display
 */
void LCD_DisplayOn(void) {
  g_displayControl |= LCD_DISPLAY_ON;
  LCD_SendCommand(LCD_CMD_DISPLAY_CTRL | g_displayControl);
}

/*
 * Description: Turn off the LCD display
 */
void LCD_DisplayOff(void) {
  g_displayControl &= ~LCD_DISPLAY_ON;
  LCD_SendCommand(LCD_CMD_DISPLAY_CTRL | g_displayControl);
}

/*
 * Description: Turn on the LCD backlight
 */
void LCD_BacklightOn(void) {
  g_backlightState = LCD_BACKLIGHT;
  I2C_WriteByte(LCD_I2C_ADDRESS, g_backlightState);
}

/*
 * Description: Turn off the LCD backlight
 */
void LCD_BacklightOff(void) {
  g_backlightState = 0;
  I2C_WriteByte(LCD_I2C_ADDRESS, g_backlightState);
}

/*
 * Description: Turn on the cursor display
 */
void LCD_CursorOn(void) {
  g_displayControl |= LCD_CURSOR_ON;
  LCD_SendCommand(LCD_CMD_DISPLAY_CTRL | g_displayControl);
}

/*
 * Description: Turn off the cursor display
 */
void LCD_CursorOff(void) {
  g_displayControl &= ~LCD_CURSOR_ON;
  LCD_SendCommand(LCD_CMD_DISPLAY_CTRL | g_displayControl);
}

/*
 * Description: Turn on cursor blinking
 */
void LCD_BlinkOn(void) {
  g_displayControl |= LCD_BLINK_ON;
  LCD_SendCommand(LCD_CMD_DISPLAY_CTRL | g_displayControl);
}

/*
 * Description: Turn off cursor blinking
 */
void LCD_BlinkOff(void) {
  g_displayControl &= ~LCD_BLINK_ON;
  LCD_SendCommand(LCD_CMD_DISPLAY_CTRL | g_displayControl);
}

/*
 * Description: Create a custom character
 * Parameters:
 *   - location: Custom character location (0-7)
 *   - pattern: Array of 8 bytes defining the character pattern
 */
void LCD_CreateCustomChar(uint8 location, const uint8 *pattern) {
  uint8 i;

  /* Only 8 custom characters (0-7) */
  location &= 0x07;

  /* Set CGRAM address */
  LCD_SendCommand(LCD_CMD_SET_CGRAM_ADDR | (location << 3));

  /* Write 8 bytes of pattern data */
  for (i = 0; i < 8; i++) {
    LCD_SendData(pattern[i]);
  }

  /* Return to DDRAM */
  LCD_SetCursor(0, 0);
}

/*
 * Description: Display a custom character
 * Parameters:
 *   - location: Custom character location (0-7)
 */
void LCD_WriteCustomChar(uint8 location) { LCD_WriteChar(location & 0x07); }

/*
 * Description: Write a signed integer to the LCD
 */
void LCD_WriteNumber(sint32 number) {
  char buffer[12]; /* Max digits for 32-bit signed int + sign + null */
  char *ptr = buffer + 11;
  uint8 isNegative = FALSE;
  uint32 absValue;

  *ptr = '\0';

  if (number < 0) {
    isNegative = TRUE;
    absValue = (uint32)(-number);
  } else {
    absValue = (uint32)number;
  }

  /* Handle zero case */
  if (absValue == 0) {
    LCD_WriteChar('0');
    return;
  }

  /* Convert number to string (reverse) */
  while (absValue > 0) {
    *--ptr = '0' + (absValue % 10);
    absValue /= 10;
  }

  if (isNegative) {
    *--ptr = '-';
  }

  LCD_WriteString(ptr);
}

/*
 * Description: Write a floating point number to the LCD
 * Parameters:
 *   - number: The float number to display
 *   - precision: Number of decimal places (0-4)
 */
void LCD_WriteFloat(float32 number, uint8 precision) {
  sint32 intPart;
  float32 fracPart;
  uint32 multiplier = 1;
  uint8 i;

  /* Handle negative numbers */
  if (number < 0) {
    LCD_WriteChar('-');
    number = -number;
  }

  /* Limit precision to 4 decimal places */
  if (precision > 4) {
    precision = 4;
  }

  /* Calculate multiplier for fractional part */
  for (i = 0; i < precision; i++) {
    multiplier *= 10;
  }

  /* Extract integer and fractional parts */
  intPart = (sint32)number;
  fracPart = number - (float32)intPart;

  /* Write integer part */
  LCD_WriteNumber(intPart);

  /* Write decimal point and fractional part if precision > 0 */
  if (precision > 0) {
    LCD_WriteChar('.');

    /* Convert fractional part */
    uint32 fracValue = (uint32)(fracPart * multiplier + 0.5f);

    /* Pad with leading zeros if needed */
    uint32 temp = multiplier / 10;
    while (temp > fracValue && temp > 1) {
      LCD_WriteChar('0');
      temp /= 10;
    }

    if (fracValue > 0) {
      LCD_WriteNumber(fracValue);
    }
  }
}

/*
 * Description: Write a hexadecimal number to the LCD
 */
void LCD_WriteHex(uint32 number) {
  char hexChars[] = "0123456789ABCDEF";
  char buffer[9]; /* 8 hex digits + null */
  char *ptr = buffer + 8;
  uint8 started = FALSE;
  sint8 i;

  *ptr = '\0';

  /* Handle zero case */
  if (number == 0) {
    LCD_WriteString("0x0");
    return;
  }

  LCD_WriteString("0x");

  /* Convert to hex string */
  for (i = 7; i >= 0; i--) {
    uint8 nibble = (number >> (i * 4)) & 0x0F;
    if (nibble != 0 || started) {
      LCD_WriteChar(hexChars[nibble]);
      started = TRUE;
    }
  }
}

/*
 * Description: Write a string at a specific position
 */
void LCD_Printf(uint8 row, uint8 col, const char *str) {
  LCD_SetCursor(row, col);
  LCD_WriteString(str);
}

/*
 * Description: Clear a specific row
 */
void LCD_ClearRow(uint8 row) {
  uint8 i;
  LCD_SetCursor(row, 0);

  for (i = 0; i < LCD_COLS; i++) {
    LCD_WriteChar(' ');
  }

  LCD_SetCursor(row, 0);
}

/*
 * Description: Scroll display one position to the left
 */
void LCD_ScrollDisplayLeft(void) {
  LCD_SendCommand(LCD_CMD_CURSOR_SHIFT | LCD_DISPLAY_MOVE | LCD_MOVE_LEFT);
}

/*
 * Description: Scroll display one position to the right
 */
void LCD_ScrollDisplayRight(void) {
  LCD_SendCommand(LCD_CMD_CURSOR_SHIFT | LCD_DISPLAY_MOVE | LCD_MOVE_RIGHT);
}
