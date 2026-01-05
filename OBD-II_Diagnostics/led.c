#include "leds.h"
#include "gpio.h"
#include "tm4c123gh6pm_registers.h"

/* LED Pin Configuration for TM4C123G LaunchPad */
#define RED_PIN      PIN1     /* PF1 */
#define BLUE_PIN     PIN2     /* PF2 */
#define GREEN_PIN    PIN3     /* PF3 */
#define LED_PORT     GPIO_PORTF

/* Unlock PORTF for configuration */
static void unlockPortF(void) {
    GPIO_PORTF_LOCK_REG = 0x4C4F434Bu; /* Unlock key */
    GPIO_PORTF_CR_REG = 0x1Fu;         /* Enable commits */
}

/* Configure pull-up for PORTF pins */
static void configurePortFPullUp(uint8 pin) {
    GPIO_PORTF_PUR_REG |= pin;
}

void LED_Init(void) {
    /* Enable clock for PORTF */
    SYSCTL_RCGCGPIO_REG |= (1u << 5u);
    
    /* Wait for clock to be ready */
    while((SYSCTL_PRGPIO_REG & (1u << 5u)) == 0u);
    
    /* Unlock PORTF */
    unlockPortF();
    
    /* Initialize LEDs as outputs */
    GPIO_InitPin(LED_PORT, RED_PIN, GPIO_OUTPUT);
    GPIO_InitPin(LED_PORT, BLUE_PIN, GPIO_OUTPUT);
    GPIO_InitPin(LED_PORT, GREEN_PIN, GPIO_OUTPUT);
    
    /* Turn off all LEDs initially */
    LED_Off(LED_ALL);
}

void LED_On(uint8 color) {
    switch(color) {
        case LED_RED:
            GPIO_WritePin(LED_PORT, RED_PIN, GPIO_HIGH);
            break;
        case LED_BLUE:
            GPIO_WritePin(LED_PORT, BLUE_PIN, GPIO_HIGH);
            break;
        case LED_GREEN:
            GPIO_WritePin(LED_PORT, GREEN_PIN, GPIO_HIGH);
            break;
        case LED_ALL:
            GPIO_WritePin(LED_PORT, RED_PIN, GPIO_HIGH);
            GPIO_WritePin(LED_PORT, BLUE_PIN, GPIO_HIGH);
            GPIO_WritePin(LED_PORT, GREEN_PIN, GPIO_HIGH);
            break;
        default:
            break;
    }
}

void LED_Off(uint8 color) {
    switch(color) {
        case LED_RED:
            GPIO_WritePin(LED_PORT, RED_PIN, GPIO_LOW);
            break;
        case LED_BLUE:
            GPIO_WritePin(LED_PORT, BLUE_PIN, GPIO_LOW);
            break;
        case LED_GREEN:
            GPIO_WritePin(LED_PORT, GREEN_PIN, GPIO_LOW);
            break;
        case LED_ALL:
            GPIO_WritePin(LED_PORT, RED_PIN, GPIO_LOW);
            GPIO_WritePin(LED_PORT, BLUE_PIN, GPIO_LOW);
            GPIO_WritePin(LED_PORT, GREEN_PIN, GPIO_LOW);
            break;
        default:
            break;
    }
}

void LED_Toggle(uint8 color) {
    switch(color) {
        case LED_RED:
            GPIO_TogglePin(LED_PORT, RED_PIN);
            break;
        case LED_BLUE:
            GPIO_TogglePin(LED_PORT, BLUE_PIN);
            break;
        case LED_GREEN:
            GPIO_TogglePin(LED_PORT, GREEN_PIN);
            break;
        case LED_ALL:
            GPIO_TogglePin(LED_PORT, RED_PIN);
            GPIO_TogglePin(LED_PORT, BLUE_PIN);
            GPIO_TogglePin(LED_PORT, GREEN_PIN);
            break;
        default:
            break;
    }
}
