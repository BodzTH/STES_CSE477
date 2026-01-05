#include "spi.h"
#include "tm4c123gh6pm_registers.h"

/* Pin Definitions for Port A (Bit-Banging) */
#define BIT_CLK  (1u << 2) /* PA2 */
#define BIT_CS   (1u << 3) /* PA3 */
#define BIT_MISO (1u << 4) /* PA4 */
#define BIT_MOSI (1u << 5) /* PA5 */

void SPI_Init(void)
{
    /* 1. Enable Port A Clock */
    SYSCTL_RCGCGPIO_REG |= 0x01;
    while((SYSCTL_PRGPIO_REG & 0x01) == 0);
    
    /* 2. Configure Output Pins (CLK, CS, MOSI) */
    GPIO_PORTA_DIR_REG |= (BIT_CLK | BIT_CS | BIT_MOSI);
    
    /* 3. Configure Input Pin (MISO) */
    GPIO_PORTA_DIR_REG &= ~BIT_MISO;
    
    /* 4. Enable Digital Functionality */
    GPIO_PORTA_DEN_REG |= (BIT_CLK | BIT_CS | BIT_MISO | BIT_MOSI);
    
    /* 5. Disable Alternate Functions (Force GPIO) */
    GPIO_PORTA_AFSEL_REG &= ~(BIT_CLK | BIT_CS | BIT_MISO | BIT_MOSI);
    GPIO_PORTA_PCTL_REG  &= 0xFF0000FF; 
    GPIO_PORTA_AMSEL_REG &= ~(BIT_CLK | BIT_CS | BIT_MISO | BIT_MOSI);
    
    /* 6. Set Idle State (Mode 0: CLK=0, CS=1) */
    GPIO_PORTA_DATA_REG &= ~BIT_CLK;
    GPIO_PORTA_DATA_REG |= BIT_CS;
}

void SPI_CS_Assert(void)   { GPIO_PORTA_DATA_REG &= ~BIT_CS; }
void SPI_CS_Deassert(void) { GPIO_PORTA_DATA_REG |= BIT_CS; }

uint8 SPI_Transfer(uint8 data)
{
    uint8 rxByte = 0;
    int i;
    
    /* SPI Mode 0 Software Implementation */
    for(i = 7; i >= 0; i--)
    {
        /* 1. Write MOSI (Setup) */
        if(data & (1u << i)) GPIO_PORTA_DATA_REG |= BIT_MOSI;
        else                 GPIO_PORTA_DATA_REG &= ~BIT_MOSI;
        
        /* 2. Clock Rising Edge (Sample) */
        GPIO_PORTA_DATA_REG |= BIT_CLK;
        
        /* 3. Read MISO */
        if(GPIO_PORTA_DATA_REG & BIT_MISO) rxByte |= (1u << i);
        
        /* 4. Clock Falling Edge (Hold) */
        GPIO_PORTA_DATA_REG &= ~BIT_CLK;
    }
    return rxByte;
}

void SPI_Write(uint8 data) { SPI_Transfer(data); }
uint8 SPI_Read(void) { return SPI_Transfer(0xFF); }
void SPI_TransferBuffer(const uint8 *txBuffer, uint8 *rxBuffer, uint16 length) { }
