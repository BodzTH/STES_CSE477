/******************************************************************************
 * Module: SPI - Bit-Banging Driver Header
 * File Name: spi.h
 * Description: Interface for Manual SPI on Port A (PA2-PA5)
 *******************************************************************************/
#ifndef SPI_H_
#define SPI_H_

#include "std_types.h"

/* Chip Select Pin Definition (PA3) */
#define SPI_CS_PIN                (1u << 3)

/* Function Prototypes */
void SPI_Init(void);
uint8 SPI_Transfer(uint8 data);
void SPI_Write(uint8 data);
uint8 SPI_Read(void);
void SPI_CS_Assert(void);
void SPI_CS_Deassert(void);
void SPI_TransferBuffer(const uint8 *txBuffer, uint8 *rxBuffer, uint16 length);

#endif /* SPI_H_ */
