/******************************************************************************
 *
 * Module: I2C
 *
 * File Name: i2c.c
 *
 * Description: Source file for I2C1 driver (PA6/PA7)
 * Fixed loop declaration for C89 compatibility
 *
 *******************************************************************************/

#include "i2c.h"
#include "tm4c123gh6pm_registers.h"
#include "delay.h"

/* System Clock Assumption: 16 MHz */
#define SYS_CLOCK_HZ            16000000

void I2C1_Init(uint32 baudRate)
{
    volatile uint32 delay;

    /* 1. Enable Clock for I2C1 and GPIO Port A */
    SYSCTL_RCGCI2C_REG |= 0x02;   /* Enable I2C1 clock */
    SYSCTL_RCGCGPIO_REG |= 0x01;  /* Enable Port A clock */
    
    /* Wait for stable clock */
    delay = SYSCTL_RCGCGPIO_REG;
    (void)delay;

    /* 2. Configure PA6 (SCL) and PA7 (SDA) */
    GPIO_PORTA_AFSEL_REG |= 0xC0; /* Enable Alt Function on PA6, PA7 */
    GPIO_PORTA_DEN_REG |= 0xC0;   /* Enable Digital on PA6, PA7 */
    
    /* Configure Open Drain for SDA (PA7) using def from i2c.h */
    GPIO_PORTA_ODR_REG |= 0x80;

    /* Configure PCTL for I2C1 (Value 3 for PA6, PA7) */
    GPIO_PORTA_PCTL_REG = (GPIO_PORTA_PCTL_REG & 0x00FFFFFF) | 0x33000000;

    /* 3. Initialize I2C1 Master */
    I2C1_MCR_REG = 0x00000010; /* Init Master function */

    /* 4. Set Timer Period for Baud Rate */
    /* TPR = (System Clock / (20 * BaudRate)) - 1 */
    I2C1_MTPR_REG = (SYS_CLOCK_HZ / (20 * baudRate)) - 1;
}

static void I2C1_Wait(void)
{
    /* Wait until the controller is not busy */
    while(I2C1_MCS_REG & I2C_MCS_BUSY);
}

void I2C1_WriteByte(uint8 slaveAddr, uint8 data)
{
    /* Set Slave Address (Writes have LSB = 0) */
    I2C1_MSA_REG = (slaveAddr << 1); 
    
    /* Set Data */
    I2C1_MDR_REG = data;

    /* Send START, RUN, STOP */
    I2C1_MCS_REG = (I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP);

    I2C1_Wait();
}

uint8 I2C1_ReadByte(uint8 slaveAddr)
{
    /* Set Slave Address (Reads have LSB = 1) */
    I2C1_MSA_REG = (slaveAddr << 1) | 1;

    /* Send START, RUN, STOP, ACK */
    I2C1_MCS_REG = (I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP | I2C_MCS_ACK);

    I2C1_Wait();

    return (uint8)I2C1_MDR_REG;
}

void I2C1_WritePacket(uint8 slaveAddr, uint8 *data, uint8 length)
{
    uint8 i; /* Variable 'i' declared at the top for C89 compatibility */

    if(length == 0) return;

    I2C1_MSA_REG = (slaveAddr << 1);
    I2C1_MDR_REG = data[0];

    if(length == 1)
    {
        I2C1_MCS_REG = (I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP);
        I2C1_Wait();
    }
    else
    {
        /* First Byte: START, RUN */
        I2C1_MCS_REG = (I2C_MCS_START | I2C_MCS_RUN);
        I2C1_Wait();

        /* Middle Bytes */
        for(i = 1; i < length - 1; i++)
        {
            I2C1_MDR_REG = data[i];
            I2C1_MCS_REG = I2C_MCS_RUN;
            I2C1_Wait();
        }

        /* Last Byte: RUN, STOP */
        I2C1_MDR_REG = data[length - 1];
        I2C1_MCS_REG = (I2C_MCS_RUN | I2C_MCS_STOP);
        I2C1_Wait();
    }
}
