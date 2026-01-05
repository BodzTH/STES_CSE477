#include "leds.h"
#include "gpio.h"
#include "tm4c123gh6pm_registers.h"

/* LED Pin Mapping */
static const struct {
    GPIO_Port port;
    uint8 pin;
} LED_CONFIG[] = {
    {GPIO_PORTF, PIN1},  /* RED   - PF1 */
    {GPIO_PORTF, PIN2},  /* BLUE  - PF2 */
    {GPIO_PORTF, PIN3}   /* GREEN - PF3 */
};

void LED_Init(void) {
    /* Initialize all LEDs as outputs */
    GPIO_InitPin(LED_CONFIG[LED_RED].port, LED_CONFIG[LED_RED].pin, GPIO_OUTPUT);
    GPIO_InitPin(LED_CONFIG[LED_BLUE].port, LED_CONFIG[LED_BLUE].pin, GPIO_OUTPUT);
    GPIO_InitPin(LED_CONFIG[LED_GREEN].port, LED_CONFIG[LED_GREEN].pin, GPIO_OUTPUT);
    
    /* Turn off all LEDs */
    LED_Off(LED_ALL);
}

void LED_On(uint8 color) {
    if(color == LED_ALL) {
        GPIO_WritePin(LED_CONFIG[LED_RED].port, LED_CONFIG[LED_RED].pin, GPIO_HIGH);
        GPIO_WritePin(LED_CONFIG[LED_BLUE].port, LED_CONFIG[LED_BLUE].pin, GPIO_HIGH);
        GPIO_WritePin(LED_CONFIG[LED_GREEN].port, LED_CONFIG[LED_GREEN].pin, GPIO_HIGH);
    } else if(color < 3) {
        GPIO_WritePin(LED_CONFIG[color].port, LED_CONFIG[color].pin, GPIO_HIGH);
    }
}

void LED_Off(uint8 color) {
    if(color == LED_ALL) {
        GPIO_WritePin(LED_CONFIG[LED_RED].port, LED_CONFIG[LED_RED].pin, GPIO_LOW);
        GPIO_WritePin(LED_CONFIG[LED_BLUE].port, LED_CONFIG[LED_BLUE].pin, GPIO_LOW);
        GPIO_WritePin(LED_CONFIG[LED_GREEN].port, LED_CONFIG[LED_GREEN].pin, GPIO_LOW);
    } else if(color < 3) {
        GPIO_WritePin(LED_CONFIG[color].port, LED_CONFIG[color].pin, GPIO_LOW);
    }
}

void LED_Toggle(uint8 color) {
    if(color == LED_ALL) {
        GPIO_TogglePin(LED_CONFIG[LED_RED].port, LED_CONFIG[LED_RED].pin);
        GPIO_TogglePin(LED_CONFIG[LED_BLUE].port, LED_CONFIG[LED_BLUE].pin);
        GPIO_TogglePin(LED_CONFIG[LED_GREEN].port, LED_CONFIG[LED_GREEN].pin);
    } else if(color < 3) {
        GPIO_TogglePin(LED_CONFIG[color].port, LED_CONFIG[color].pin);
    }
}

