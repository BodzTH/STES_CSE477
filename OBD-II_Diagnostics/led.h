#ifndef LEDS_H
#define LEDS_H

#include "std_types.h"

/* LED Colors */
typedef enum {
    LED_RED,
    LED_BLUE,
    LED_GREEN,
    LED_ALL
} LED_Color;

/* LED Functions */
void LED_Init(void);
void LED_On(uint8 color);
void LED_Off(uint8 color);
void LED_Toggle(uint8 color);

/* Simple Macros */
#define RED_ON()      LED_On(LED_RED)
#define RED_OFF()     LED_Off(LED_RED)
#define RED_TOGGLE()  LED_Toggle(LED_RED)

#define BLUE_ON()     LED_On(LED_BLUE)
#define BLUE_OFF()    LED_Off(LED_BLUE)
#define BLUE_TOGGLE() LED_Toggle(LED_BLUE)

#define GREEN_ON()    LED_On(LED_GREEN)
#define GREEN_OFF()   LED_Off(LED_GREEN)
#define GREEN_TOGGLE() LED_Toggle(LED_GREEN)

#endif
