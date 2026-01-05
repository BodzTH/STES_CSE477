#ifndef PUSHBUTTON_H
#define PUSHBUTTON_H

#include "std_types.h"

#define SW1_PIN     0x10
#define SW1_PRESSED 0

void Button_Init(void);
uint8_t Button_Read(void);
uint8_t Button_IsPressed(void);
uint8_t Button_WasPressed(void);
uint8_t Button_WasReleased(void);
void Button_EnableInterrupt(void (*handler)(void));
void Button_DisableInterrupt(void);

#define SW1_READ()      Button_Read()
#define SW1_PRESSED()   Button_IsPressed()

#endif

