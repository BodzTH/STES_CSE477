#ifndef LEDS_H
#define LEDS_H

#include <stdint.h>
#include "tm4c123gh6pm.h"

#define RED_LED    0x02
#define BLUE_LED   0x04
#define GREEN_LED  0x08

void LED_Init(void);
void LED_On(uint8_t color);
void LED_Off(uint8_t color);
void LED_Toggle(uint8_t color);
void LED_Set(uint8_t color, uint8_t state);
void LED_AllOff(void);

#define RED_ON()        LED_On(RED_LED)
#define RED_OFF()       LED_Off(RED_LED)
#define RED_TOGGLE()    LED_Toggle(RED_LED)

#define BLUE_ON()       LED_On(BLUE_LED)
#define BLUE_OFF()      LED_Off(BLUE_LED)
#define BLUE_TOGGLE()   LED_Toggle(BLUE_LED)

#define GREEN_ON()      LED_On(GREEN_LED)
#define GREEN_OFF()     LED_Off(GREEN_LED)
#define GREEN_TOGGLE()  LED_Toggle(GREEN_LED)

#endif
