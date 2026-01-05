#ifndef MCP2515_H_
#define MCP2515_H_

#include "std_types.h"

/* Commands */
#define MCP2515_CMD_RESET           0xC0
#define MCP2515_CMD_READ            0x03
#define MCP2515_CMD_WRITE           0x02
#define MCP2515_CMD_READ_RX0        0x90
#define MCP2515_CMD_READ_RX1        0x94
#define MCP2515_CMD_LOAD_TX0        0x40
#define MCP2515_CMD_RTS_TX0         0x81
#define MCP2515_CMD_BIT_MODIFY      0x05

/* Registers */
#define MCP2515_REG_CANSTAT         0x0E
#define MCP2515_REG_CANCTRL         0x0F
#define MCP2515_REG_CNF1            0x2A
#define MCP2515_REG_CNF2            0x29
#define MCP2515_REG_CNF3            0x28
#define MCP2515_REG_CANINTE         0x2B
#define MCP2515_REG_CANINTF         0x2C
#define MCP2515_REG_RXB0CTRL        0x60
#define MCP2515_REG_RXB1CTRL        0x70

/* Status Codes */
#define MCP2515_STATUS_OK           0
#define MCP2515_STATUS_ERROR        1
#define MCP2515_STATUS_TIMEOUT      2
#define MCP2515_STATUS_NO_MSG       4

/* Config Types */
typedef struct {
    uint8 baudRate;     /* 0=500kbps */
    boolean loopbackMode;
} MCP2515_Config;

typedef struct {
    uint32 id;
    uint8  idType;      /* 0=Std, 1=Ext */
    uint8  dlc;
    uint8  data[8];
} MCP2515_Message;

#define MCP2515_BAUD_500KBPS        0
#define MCP2515_FRAME_STD           0
#define MCP2515_FRAME_EXT           1

/* Function Prototypes */
uint8 MCP2515_Init(const MCP2515_Config *config);
void MCP2515_Reset(void);
uint8 MCP2515_ReadRegister(uint8 address);
void MCP2515_WriteRegister(uint8 address, uint8 value);
void MCP2515_BitModify(uint8 address, uint8 mask, uint8 value);
uint8 MCP2515_Transmit(const MCP2515_Message *msg);
uint8 MCP2515_Receive(MCP2515_Message *msg);
uint8 MCP2515_ReceiveWithTimeout(MCP2515_Message *msg, uint32 timeout_ms);
uint8 MCP2515_ConfigureMask(uint8 maskNum, uint32 mask, uint8 idType);
uint8 MCP2515_ConfigureFilter(uint8 filterNum, uint32 id, uint8 idType);

#endif /* MCP2515_H_ */
