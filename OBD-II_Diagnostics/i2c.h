/******************************************************************************
 *
 * Module: I2C
 *
 * File Name: i2c.h
 *
 * Description: Header file for I2C1 driver (PA6/PA7)
 * Includes Register Definitions for I2C1 and Port A ODR
 *
 *******************************************************************************/

#ifndef I2C_H_
#define I2C_H_

#include "std_types.h"

/*******************************************************************************
 * Register Definitions (I2C1 & ODR)                      *
 *******************************************************************************/

/* Port A Open Drain Register (Missing from main register file) */
#ifndef GPIO_PORTA_ODR_REG
#define GPIO_PORTA_ODR_REG      (*((volatile uint32 *)0x4000450C))
#endif

/* I2C1 Base Registers (Base: 0x4002.1000) */
#define I2C1_MSA_REG            (*((volatile uint32 *)0x40021000))
#define I2C1_MCS_REG            (*((volatile uint32 *)0x40021004))
#define I2C1_MDR_REG            (*((volatile uint32 *)0x40021008))
#define I2C1_MTPR_REG           (*((volatile uint32 *)0x4002100C))
#define I2C1_MCR_REG            (*((volatile uint32 *)0x40021020))

/* I2C Control Bits */
#define I2C_MCS_RUN             0x00000001  /* I2C Master Enable */
#define I2C_MCS_START           0x00000002  /* Generate START */
#define I2C_MCS_STOP            0x00000004  /* Generate STOP */
#define I2C_MCS_ACK             0x00000008  /* Data Acknowledge Enable */
#define I2C_MCS_BUSY            0x00000001  /* I2C Busy */
#define I2C_MCS_ERROR           0x00000002  /* Error */

/*******************************************************************************
 * Definitions                                   *
 *******************************************************************************/
#define I2C_BAUDRATE_100K    100000
#define I2C_BAUDRATE_400K    400000

/*******************************************************************************
 * Function Prototypes                               *
 *******************************************************************************/

/* Initialize I2C1 module on PA6 (SCL) and PA7 (SDA) */
void I2C1_Init(uint32 baudRate);

/* Write a single byte to a specific slave address */
void I2C1_WriteByte(uint8 slaveAddr, uint8 data);

/* Read a single byte from a specific slave address */
uint8 I2C1_ReadByte(uint8 slaveAddr);

/* Write multiple bytes (used for LCD commands) */
void I2C1_WritePacket(uint8 slaveAddr, uint8 *data, uint8 length);

#endif /* I2C_H_ */
