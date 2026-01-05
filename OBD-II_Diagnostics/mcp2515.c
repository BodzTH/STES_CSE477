#include "mcp2515.h"
#include "spi.h"
#include "delay.h"

void MCP2515_Reset(void)
{
    SPI_CS_Assert();
    SPI_Write(MCP2515_CMD_RESET);
    SPI_CS_Deassert();
    Delay_MS(10);
}

uint8 MCP2515_ReadRegister(uint8 address)
{
    uint8 val;
    SPI_CS_Assert();
    SPI_Write(MCP2515_CMD_READ);
    SPI_Write(address);
    val = SPI_Read();
    SPI_CS_Deassert();
    return val;
}

void MCP2515_WriteRegister(uint8 address, uint8 value)
{
    SPI_CS_Assert();
    SPI_Write(MCP2515_CMD_WRITE);
    SPI_Write(address);
    SPI_Write(value);
    SPI_CS_Deassert();
}

void MCP2515_BitModify(uint8 address, uint8 mask, uint8 value)
{
    SPI_CS_Assert();
    SPI_Write(MCP2515_CMD_BIT_MODIFY);
    SPI_Write(address);
    SPI_Write(mask);
    SPI_Write(value);
    SPI_CS_Deassert();
}

uint8 MCP2515_Init(const MCP2515_Config *config)
{
    uint8 status;
    
    /* 1. Init SPI */
    SPI_Init();
    
    /* 2. Reset MCP2515 */
    MCP2515_Reset();
    
    /* 3. SANITY CHECK: Must be 0x80 after reset */
    status = MCP2515_ReadRegister(MCP2515_REG_CANSTAT);
    if((status & 0xE0) != 0x80)
    {
        return MCP2515_STATUS_ERROR; 
    }
    
    /* 4. Configure Baud Rate (500kbps) */
    MCP2515_WriteRegister(MCP2515_REG_CNF1, 0x00);
    MCP2515_WriteRegister(MCP2515_REG_CNF2, 0x90);
    MCP2515_WriteRegister(MCP2515_REG_CNF3, 0x02);
    
    /* 5. Clear Interrupts */
    MCP2515_WriteRegister(MCP2515_REG_CANINTF, 0x00);
    
    /* 6. FORCE LOOPBACK MODE (For Desk Testing) */
    /* 0x40 = Loopback. 0x00 = Normal */
    MCP2515_BitModify(MCP2515_REG_CANCTRL, 0xE0, 0x40);
    
    /* 7. Verify Loopback Mode */
    Delay_MS(10);
    status = MCP2515_ReadRegister(MCP2515_REG_CANSTAT);
    
    /* We now expect 0x40 (Loopback), NOT 0x00 */
    if((status & 0xE0) != 0x40)
    {
        return MCP2515_STATUS_ERROR;
    }
    
    return MCP2515_STATUS_OK;
}

uint8 MCP2515_Transmit(const MCP2515_Message *msg)
{
    uint8 i;
    uint8 ctrl = MCP2515_ReadRegister(0x30);
    if(ctrl & 0x08) return MCP2515_STATUS_TIMEOUT; 
    
    SPI_CS_Assert();
    SPI_Write(MCP2515_CMD_LOAD_TX0);
    SPI_Write((uint8)(msg->id >> 3));
    SPI_Write((uint8)((msg->id & 0x07) << 5));
    SPI_Write(0x00);
    SPI_Write(0x00);
    SPI_Write(msg->dlc);
    for(i=0; i<msg->dlc; i++) SPI_Write(msg->data[i]);
    SPI_CS_Deassert();
    
    SPI_CS_Assert();
    SPI_Write(MCP2515_CMD_RTS_TX0);
    SPI_CS_Deassert();
    
    return MCP2515_STATUS_OK;
}

uint8 MCP2515_Receive(MCP2515_Message *msg)
{
    uint8 intf, i;
    intf = MCP2515_ReadRegister(MCP2515_REG_CANINTF);
    
    if(intf & 0x01)
    {
        SPI_CS_Assert();
        SPI_Write(MCP2515_CMD_READ_RX0);
        uint8 d0 = SPI_Read();
        uint8 d1 = SPI_Read();
        SPI_Read(); SPI_Read();
        uint8 dlc = SPI_Read();
        msg->id = ((uint16)d0 << 3) | (d1 >> 5);
        msg->dlc = dlc & 0x0F;
        for(i=0; i<msg->dlc; i++) msg->data[i] = SPI_Read();
        SPI_CS_Deassert();
        
        MCP2515_BitModify(MCP2515_REG_CANINTF, 0x01, 0x00);
        return MCP2515_STATUS_OK;
    }
    return MCP2515_STATUS_NO_MSG;
}

uint8 MCP2515_ReceiveWithTimeout(MCP2515_Message *msg, uint32 timeout_ms)
{
    while(timeout_ms--)
    {
        if(MCP2515_Receive(msg) == MCP2515_STATUS_OK) return MCP2515_STATUS_OK;
        Delay_MS(1);
    }
    return MCP2515_STATUS_TIMEOUT;
}

/* Dummy stubs */
uint8 MCP2515_ConfigureMask(uint8 maskNum, uint32 mask, uint8 idType) { return 0; }
uint8 MCP2515_ConfigureFilter(uint8 filterNum, uint32 id, uint8 idType) { return 0; }
uint8 MCP2515_MessageAvailable(void) { return 0; }
