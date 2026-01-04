/******************************************************************************
*
* Module: I2C - Inter-Integrated Circuit Driver
*
* File Name: i2c.c
*
* Description: Source file for I2C driver for TM4C123GH6PM
*
* Target: TM4C123GH6PM (Tiva C LaunchPad)
*
* I2C0 Pins: PB2 (SCL), PB3 (SDA)
* I2C1 Pins: PA6 (SCL), PA7 (SDA)
* I2C2 Pins: PE4 (SCL), PE5 (SDA)
* I2C3 Pins: PD0 (SCL), PD1 (SDA)
*
*******************************************************************************/

#include "i2c.h"
#include "tm4c123gh6pm_registers.h"
#include "delay.h"

/*******************************************************************************
 *                                Definitions                                   *
 *******************************************************************************/

/* System Clock Frequency (assuming 16 MHz) */
#define SYSTEM_CLOCK_FREQ       16000000

/* I2C Timeout (in iterations) */
#define I2C_TIMEOUT             10000

/* I2C0 Base Registers */
#define I2C0_MSA_R              (*((volatile uint32*)0x40020000))
#define I2C0_MCS_R              (*((volatile uint32*)0x40020004))
#define I2C0_MDR_R              (*((volatile uint32*)0x40020008))
#define I2C0_MTPR_R             (*((volatile uint32*)0x4002000C))
#define I2C0_MCR_R              (*((volatile uint32*)0x40020020))

/*******************************************************************************
 *                              Private Variables                               *
 *******************************************************************************/

static uint8 g_currentModule = I2C_MODULE_0;

/*******************************************************************************
 *                              Private Functions                               *
 *******************************************************************************/

/*
 * Description: Wait for I2C master to become idle
 */
static I2C_Error I2C_WaitMasterIdle(void)
{
    uint32 timeout = I2C_TIMEOUT;
    
    /* Wait while busy */
    while (I2C0_MCS_R & I2C_MCS_BUSY)
    {
        if (--timeout == 0)
        {
            return I2C_ERROR_TIMEOUT;
        }
    }
    
    return I2C_OK;
}

/*
 * Description: Check for errors after I2C operation
 */
static I2C_Error I2C_CheckError(void)
{
    uint32 status = I2C0_MCS_R;
    
    if (status & I2C_MCS_ERROR)
    {
        if (status & I2C_MCS_ARBLST)
        {
            return I2C_ERROR_ARB_LOST;
        }
        if (status & I2C_MCS_ADRACK)
        {
            return I2C_ERROR_ADDRESS_NACK;
        }
        if (status & I2C_MCS_DATACK)
        {
            return I2C_ERROR_DATA_NACK;
        }
    }
    
    return I2C_OK;
}

/*******************************************************************************
 *                              Public Functions                                *
 *******************************************************************************/

/*
 * Description: Initialize I2C0 with default settings (100 kHz)
 */
void I2C_Init(void)
{
    I2C_InitModule(I2C_MODULE_0, I2C_SPEED_STANDARD);
}

/*
 * Description: Initialize specified I2C module
 * Parameters:
 *   - module: I2C module number (0-3)
 *   - speed: I2C speed in Hz
 */
void I2C_InitModule(uint8 module, uint32 speed)
{
    uint32 tpr;
    
    g_currentModule = module;
    
    /* Enable I2C0 and GPIO Port B clocks */
    SYSCTL_RCGCI2C_REG |= (1 << module);    /* Enable I2C module clock */
    SYSCTL_RCGCGPIO_REG |= 0x02;            /* Enable Port B clock (for I2C0) */
    
    /* Wait for clocks to stabilize */
    delay_us(10);
    
    /* Configure GPIO pins for I2C0 (PB2 = SCL, PB3 = SDA) */
    GPIO_PORTB_AFSEL_REG |= 0x0C;           /* Enable alternate function on PB2, PB3 */
    GPIO_PORTB_ODR_REG |= 0x08;             /* Enable open-drain on PB3 (SDA) */
    GPIO_PORTB_DEN_REG |= 0x0C;             /* Enable digital on PB2, PB3 */
    GPIO_PORTB_PCTL_REG &= ~0x0000FF00;     /* Clear PCTL for PB2, PB3 */
    GPIO_PORTB_PCTL_REG |= 0x00003300;      /* Set PCTL for I2C function (PMC = 3) */
    
    /* Initialize I2C Master */
    I2C0_MCR_R = 0x10;                      /* Initialize I2C Master */
    
    /* Calculate Timer Period Register value */
    /* TPR = (System Clock / (2 * (SCL_LP + SCL_HP) * SCL_CLK)) - 1 */
    /* For standard mode: SCL_LP = 6, SCL_HP = 4 */
    /* TPR = (16MHz / (2 * 10 * 100kHz)) - 1 = 7 */
    tpr = (SYSTEM_CLOCK_FREQ / (2 * 10 * speed)) - 1;
    
    /* Set Timer Period */
    I2C0_MTPR_R = tpr & 0x7F;               /* Set TPR (7 bits only) */
}

/*
 * Description: Write a single byte to I2C slave
 */
I2C_Error I2C_WriteByte(uint8 slaveAddr, uint8 data)
{
    I2C_Error error;
    
    /* Wait for I2C to be ready */
    error = I2C_WaitMasterIdle();
    if (error != I2C_OK) return error;
    
    /* Set slave address with write bit (R/W = 0) */
    I2C0_MSA_R = (slaveAddr << 1) & 0xFE;
    
    /* Put data to send */
    I2C0_MDR_R = data;
    
    /* Send START + Data + STOP (single byte transfer) */
    I2C0_MCS_R = I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP;
    
    /* Wait for operation to complete */
    error = I2C_WaitMasterIdle();
    if (error != I2C_OK) return error;
    
    /* Check for errors */
    return I2C_CheckError();
}

/*
 * Description: Read a single byte from I2C slave
 */
I2C_Error I2C_ReadByte(uint8 slaveAddr, uint8* data)
{
    I2C_Error error;
    
    /* Wait for I2C to be ready */
    error = I2C_WaitMasterIdle();
    if (error != I2C_OK) return error;
    
    /* Set slave address with read bit (R/W = 1) */
    I2C0_MSA_R = ((slaveAddr << 1) & 0xFE) | 0x01;
    
    /* Send START + Read + NACK + STOP (single byte receive) */
    I2C0_MCS_R = I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP;
    
    /* Wait for operation to complete */
    error = I2C_WaitMasterIdle();
    if (error != I2C_OK) return error;
    
    /* Check for errors */
    error = I2C_CheckError();
    if (error != I2C_OK) return error;
    
    /* Read received data */
    *data = I2C0_MDR_R & 0xFF;
    
    return I2C_OK;
}

/*
 * Description: Write multiple bytes to I2C slave
 */
I2C_Error I2C_WriteMultiple(uint8 slaveAddr, uint8* data, uint8 length)
{
    I2C_Error error;
    uint8 i;
    
    if (length == 0) return I2C_OK;
    
    /* Wait for I2C to be ready */
    error = I2C_WaitMasterIdle();
    if (error != I2C_OK) return error;
    
    /* Set slave address with write bit (R/W = 0) */
    I2C0_MSA_R = (slaveAddr << 1) & 0xFE;
    
    /* Send first byte with START */
    I2C0_MDR_R = data[0];
    
    if (length == 1)
    {
        /* Single byte: START + Data + STOP */
        I2C0_MCS_R = I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP;
    }
    else
    {
        /* Multi-byte: START + Data (no STOP) */
        I2C0_MCS_R = I2C_MCS_START | I2C_MCS_RUN;
    }
    
    error = I2C_WaitMasterIdle();
    if (error != I2C_OK) return error;
    
    error = I2C_CheckError();
    if (error != I2C_OK) return error;
    
    /* Send remaining bytes */
    for (i = 1; i < length; i++)
    {
        I2C0_MDR_R = data[i];
        
        if (i == length - 1)
        {
            /* Last byte: Data + STOP */
            I2C0_MCS_R = I2C_MCS_RUN | I2C_MCS_STOP;
        }
        else
        {
            /* Middle byte: Data (no STOP) */
            I2C0_MCS_R = I2C_MCS_RUN;
        }
        
        error = I2C_WaitMasterIdle();
        if (error != I2C_OK) return error;
        
        error = I2C_CheckError();
        if (error != I2C_OK) return error;
    }
    
    return I2C_OK;
}

/*
 * Description: Read multiple bytes from I2C slave
 */
I2C_Error I2C_ReadMultiple(uint8 slaveAddr, uint8* data, uint8 length)
{
    I2C_Error error;
    uint8 i;
    
    if (length == 0) return I2C_OK;
    
    /* Wait for I2C to be ready */
    error = I2C_WaitMasterIdle();
    if (error != I2C_OK) return error;
    
    /* Set slave address with read bit (R/W = 1) */
    I2C0_MSA_R = ((slaveAddr << 1) & 0xFE) | 0x01;
    
    if (length == 1)
    {
        /* Single byte: START + Read + NACK + STOP */
        I2C0_MCS_R = I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP;
        
        error = I2C_WaitMasterIdle();
        if (error != I2C_OK) return error;
        
        error = I2C_CheckError();
        if (error != I2C_OK) return error;
        
        data[0] = I2C0_MDR_R & 0xFF;
    }
    else
    {
        /* First byte: START + Read + ACK */
        I2C0_MCS_R = I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_ACK;
        
        error = I2C_WaitMasterIdle();
        if (error != I2C_OK) return error;
        
        error = I2C_CheckError();
        if (error != I2C_OK) return error;
        
        data[0] = I2C0_MDR_R & 0xFF;
        
        /* Middle bytes */
        for (i = 1; i < length - 1; i++)
        {
            I2C0_MCS_R = I2C_MCS_RUN | I2C_MCS_ACK;
            
            error = I2C_WaitMasterIdle();
            if (error != I2C_OK) return error;
            
            error = I2C_CheckError();
            if (error != I2C_OK) return error;
            
            data[i] = I2C0_MDR_R & 0xFF;
        }
        
        /* Last byte: Read + NACK + STOP */
        I2C0_MCS_R = I2C_MCS_RUN | I2C_MCS_STOP;
        
        error = I2C_WaitMasterIdle();
        if (error != I2C_OK) return error;
        
        error = I2C_CheckError();
        if (error != I2C_OK) return error;
        
        data[length - 1] = I2C0_MDR_R & 0xFF;
    }
    
    return I2C_OK;
}

/*
 * Description: Write a byte to a specific register
 */
I2C_Error I2C_WriteRegister(uint8 slaveAddr, uint8 regAddr, uint8 data)
{
    uint8 buffer[2];
    buffer[0] = regAddr;
    buffer[1] = data;
    return I2C_WriteMultiple(slaveAddr, buffer, 2);
}

/*
 * Description: Read a byte from a specific register
 */
I2C_Error I2C_ReadRegister(uint8 slaveAddr, uint8 regAddr, uint8* data)
{
    I2C_Error error;
    
    /* Write register address */
    error = I2C_WaitMasterIdle();
    if (error != I2C_OK) return error;
    
    I2C0_MSA_R = (slaveAddr << 1) & 0xFE;   /* Write mode */
    I2C0_MDR_R = regAddr;
    I2C0_MCS_R = I2C_MCS_START | I2C_MCS_RUN;   /* START + Write (no STOP for repeated start) */
    
    error = I2C_WaitMasterIdle();
    if (error != I2C_OK) return error;
    
    error = I2C_CheckError();
    if (error != I2C_OK) return error;
    
    /* Read data with repeated START */
    I2C0_MSA_R = ((slaveAddr << 1) & 0xFE) | 0x01;   /* Read mode */
    I2C0_MCS_R = I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP;
    
    error = I2C_WaitMasterIdle();
    if (error != I2C_OK) return error;
    
    error = I2C_CheckError();
    if (error != I2C_OK) return error;
    
    *data = I2C0_MDR_R & 0xFF;
    
    return I2C_OK;
}

/*
 * Description: Write multiple bytes to consecutive registers
 */
I2C_Error I2C_WriteRegisterMultiple(uint8 slaveAddr, uint8 regAddr, uint8* data, uint8 length)
{
    I2C_Error error;
    uint8 i;
    
    if (length == 0) return I2C_OK;
    
    /* Wait for I2C to be ready */
    error = I2C_WaitMasterIdle();
    if (error != I2C_OK) return error;
    
    /* Set slave address with write bit */
    I2C0_MSA_R = (slaveAddr << 1) & 0xFE;
    
    /* Send register address with START */
    I2C0_MDR_R = regAddr;
    I2C0_MCS_R = I2C_MCS_START | I2C_MCS_RUN;
    
    error = I2C_WaitMasterIdle();
    if (error != I2C_OK) return error;
    
    error = I2C_CheckError();
    if (error != I2C_OK) return error;
    
    /* Send data bytes */
    for (i = 0; i < length; i++)
    {
        I2C0_MDR_R = data[i];
        
        if (i == length - 1)
        {
            I2C0_MCS_R = I2C_MCS_RUN | I2C_MCS_STOP;
        }
        else
        {
            I2C0_MCS_R = I2C_MCS_RUN;
        }
        
        error = I2C_WaitMasterIdle();
        if (error != I2C_OK) return error;
        
        error = I2C_CheckError();
        if (error != I2C_OK) return error;
    }
    
    return I2C_OK;
}

/*
 * Description: Read multiple bytes from consecutive registers
 */
I2C_Error I2C_ReadRegisterMultiple(uint8 slaveAddr, uint8 regAddr, uint8* data, uint8 length)
{
    I2C_Error error;
    
    /* Write register address first */
    error = I2C_WaitMasterIdle();
    if (error != I2C_OK) return error;
    
    I2C0_MSA_R = (slaveAddr << 1) & 0xFE;   /* Write mode */
    I2C0_MDR_R = regAddr;
    I2C0_MCS_R = I2C_MCS_START | I2C_MCS_RUN;   /* No STOP for repeated start */
    
    error = I2C_WaitMasterIdle();
    if (error != I2C_OK) return error;
    
    error = I2C_CheckError();
    if (error != I2C_OK) return error;
    
    /* Set slave address for read */
    I2C0_MSA_R = ((slaveAddr << 1) & 0xFE) | 0x01;
    
    /* Read data bytes */
    return I2C_ReadMultiple(slaveAddr, data, length);
}

/*
 * Description: Check if I2C bus is busy
 */
boolean I2C_IsBusy(void)
{
    return (I2C0_MCS_R & I2C_MCS_BUSBSY) ? TRUE : FALSE;
}

/*
 * Description: Wait until I2C is ready
 */
I2C_Error I2C_WaitUntilReady(void)
{
    return I2C_WaitMasterIdle();
}

/*
 * Description: Scan for a specific device on the I2C bus
 */
boolean I2C_ScanDevice(uint8 slaveAddr)
{
    I2C_Error error;
    
    /* Wait for I2C to be ready */
    error = I2C_WaitMasterIdle();
    if (error != I2C_OK) return FALSE;
    
    /* Set slave address with write bit */
    I2C0_MSA_R = (slaveAddr << 1) & 0xFE;
    
    /* Just send START + address + STOP */
    I2C0_MDR_R = 0x00;
    I2C0_MCS_R = I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP;
    
    /* Wait for operation to complete */
    error = I2C_WaitMasterIdle();
    if (error != I2C_OK) return FALSE;
    
    /* Check if device acknowledged */
    if (I2C0_MCS_R & I2C_MCS_ERROR)
    {
        return FALSE;
    }
    
    return TRUE;
}

/*
 * Description: Scan entire I2C bus for devices
 * Parameters:
 *   - foundDevices: Array to store found device addresses
 *   - count: Pointer to store number of devices found
 */
void I2C_ScanBus(uint8* foundDevices, uint8* count)
{
    uint8 addr;
    *count = 0;
    
    /* Scan addresses from 0x08 to 0x77 (valid 7-bit address range) */
    for (addr = 0x08; addr < 0x78; addr++)
    {
        if (I2C_ScanDevice(addr))
        {
            foundDevices[*count] = addr;
            (*count)++;
        }
        delay_ms(1);
    }
}
