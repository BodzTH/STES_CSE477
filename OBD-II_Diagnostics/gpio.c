#include "gpio.h"
#include "tm4c123gh6pm_registers.h"

/* System Control Registers */
#define SYSCTL_RCGCGPIO_R  (*((volatile uint32 *)0x400FE608))
#define SYSCTL_PRGPIO_R    (*((volatile uint32 *)0x400FEA08))

/* GPIO Base Addresses */
static volatile uint32* GPIO_BASE[] = {
    (volatile uint32*)0x40004000,  /* PORTA */
    (volatile uint32*)0x40005000,  /* PORTB */
    (volatile uint32*)0x40006000,  /* PORTC */
    (volatile uint32*)0x40007000,  /* PORTD */
    (volatile uint32*)0x40024000,  /* PORTE */
    (volatile uint32*)0x40025000   /* PORTF */
};

/* Register Offsets */
#define DATA_OFFSET   0
#define DIR_OFFSET    1
#define DEN_OFFSET    5

void GPIO_InitPin(GPIO_Port port, uint8 pin, uint8 direction) {
    /* Enable clock for the port */
    SYSCTL_RCGCGPIO_R |= (1 << port);
    
    /* Wait for clock to be ready */
    while((SYSCTL_PRGPIO_R & (1 << port)) == 0);
    
    volatile uint32* portBase = GPIO_BASE[port];
    
    /* Set direction */
    if(direction == GPIO_OUTPUT) {
        portBase[DIR_OFFSET] |= pin;
    } else {
        portBase[DIR_OFFSET] &= ~pin;
    }
    
    /* Enable digital function */
    portBase[DEN_OFFSET] |= pin;
}

void GPIO_WritePin(GPIO_Port port, uint8 pin, uint8 level) {
    volatile uint32* portBase = GPIO_BASE[port];
    
    if(level == GPIO_HIGH) {
        portBase[DATA_OFFSET] |= pin;
    } else {
        portBase[DATA_OFFSET] &= ~pin;
    }
}

uint8 GPIO_ReadPin(GPIO_Port port, uint8 pin) {
    volatile uint32* portBase = GPIO_BASE[port];
    
    if(portBase[DATA_OFFSET] & pin) {
        return GPIO_HIGH;
    } else {
        return GPIO_LOW;
    }
}

void GPIO_TogglePin(GPIO_Port port, uint8 pin) {
    volatile uint32* portBase = GPIO_BASE[port];
    portBase[DATA_OFFSET] ^= pin;
}
