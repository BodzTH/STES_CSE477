/******************************************************************************
*
* Module: I2C - Inter-Integrated Circuit Driver
*
* File Name: i2c.h
*
* Description: Header file for I2C driver for TM4C123GH6PM
*
* Target: TM4C123GH6PM (Tiva C LaunchPad)
*
*******************************************************************************/

#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>
#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                   *
 *******************************************************************************/

/* I2C Module Selection */
#define I2C_MODULE_0            0
#define I2C_MODULE_1            1
#define I2C_MODULE_2            2
#define I2C_MODULE_3            3

/* Default I2C Module to use */
#define I2C_DEFAULT_MODULE      I2C_MODULE_0

/* I2C Speed Modes */
#define I2C_SPEED_STANDARD      100000      /* 100 kHz */
#define I2C_SPEED_FAST          400000      /* 400 kHz */
#define I2C_SPEED_FAST_PLUS     1000000     /* 1 MHz */

/* I2C Master Control/Status Register Bits */
#define I2C_MCS_BUSY            (1 << 0)    /* I2C Busy */
#define I2C_MCS_ERROR           (1 << 1)    /* Error */
#define I2C_MCS_ADRACK          (1 << 2)    /* Acknowledge Address */
#define I2C_MCS_DATACK          (1 << 3)    /* Acknowledge Data */
#define I2C_MCS_ARBLST          (1 << 4)    /* Arbitration Lost */
#define I2C_MCS_IDLE            (1 << 5)    /* I2C Idle */
#define I2C_MCS_BUSBSY          (1 << 6)    /* Bus Busy */

/* I2C Master Control Register Commands */
#define I2C_MCS_RUN             (1 << 0)    /* I2C Master Enable */
#define I2C_MCS_START           (1 << 1)    /* Generate START */
#define I2C_MCS_STOP            (1 << 2)    /* Generate STOP */
#define I2C_MCS_ACK             (1 << 3)    /* Data Acknowledge Enable */
#define I2C_MCS_HS              (1 << 4)    /* High-Speed Enable */

/* I2C Error Codes */
typedef enum {
    I2C_OK = 0,
    I2C_ERROR_ADDRESS_NACK,
    I2C_ERROR_DATA_NACK,
    I2C_ERROR_ARB_LOST,
    I2C_ERROR_TIMEOUT,
    I2C_ERROR_BUSY
} I2C_Error;

/*******************************************************************************
 *                              Function Prototypes                             *
 *******************************************************************************/

/* Initialization Functions */
void I2C_Init(void);
void I2C_InitModule(uint8 module, uint32 speed);

/* Single Byte Operations */
I2C_Error I2C_WriteByte(uint8 slaveAddr, uint8 data);
I2C_Error I2C_ReadByte(uint8 slaveAddr, uint8* data);

/* Multi-Byte Operations */
I2C_Error I2C_WriteMultiple(uint8 slaveAddr, uint8* data, uint8 length);
I2C_Error I2C_ReadMultiple(uint8 slaveAddr, uint8* data, uint8 length);

/* Register Operations */
I2C_Error I2C_WriteRegister(uint8 slaveAddr, uint8 regAddr, uint8 data);
I2C_Error I2C_ReadRegister(uint8 slaveAddr, uint8 regAddr, uint8* data);
I2C_Error I2C_WriteRegisterMultiple(uint8 slaveAddr, uint8 regAddr, uint8* data, uint8 length);
I2C_Error I2C_ReadRegisterMultiple(uint8 slaveAddr, uint8 regAddr, uint8* data, uint8 length);

/* Utility Functions */
boolean I2C_IsBusy(void);
I2C_Error I2C_WaitUntilReady(void);
boolean I2C_ScanDevice(uint8 slaveAddr);
void I2C_ScanBus(uint8* foundDevices, uint8* count);

#endif /* I2C_H_ */
