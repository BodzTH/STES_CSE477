#include "obd.h"
#include "mcp2515.h"
#include "delay.h"

uint8 OBD_Init(void)
{
    MCP2515_Config cfg;
    cfg.baudRate = MCP2515_BAUD_500KBPS;
    
    if(MCP2515_Init(&cfg) != MCP2515_STATUS_OK)
    {
        return OBD_STATUS_ERROR;
    }
    return OBD_STATUS_OK;
}

static uint8 OBD_Request(uint8 pid, uint8 *dataOut, uint8 len)
{
    MCP2515_Message msg;
    uint8 status;
    
    /* 1. Send Request */
    msg.id = OBD_REQUEST_ID;
    msg.dlc = 8;
    msg.data[0] = 0x02; /* Length */
    msg.data[1] = 0x01; /* Mode 1 */
    msg.data[2] = pid;
    msg.data[3] = 0x55; /* Padding */
    msg.data[4] = 0x55;
    msg.data[5] = 0x55;
    msg.data[6] = 0x55;
    msg.data[7] = 0x55;
    
    if(MCP2515_Transmit(&msg) != MCP2515_STATUS_OK) return OBD_STATUS_ERROR;
    
    /* 2. Wait for Response */
    status = MCP2515_ReceiveWithTimeout(&msg, 100); /* 100ms Timeout */
    
    if(status == MCP2515_STATUS_OK)
    {
        /* Check if it is a response to our Mode 1 request */
        if(msg.data[1] == 0x41 && msg.data[2] == pid)
        {
            uint8 i;
            for(i=0; i<len; i++) dataOut[i] = msg.data[3+i];
            return OBD_STATUS_OK;
        }
    }
    return OBD_STATUS_TIMEOUT;
}

uint8 OBD_GetEngineRPM(uint16 *rpm)
{
    uint8 data[2];
    if(OBD_Request(OBD_PID_RPM, data, 2) == OBD_STATUS_OK)
    {
        *rpm = ((uint16)data[0] * 256 + data[1]) / 4;
        return OBD_STATUS_OK;
    }
    return OBD_STATUS_ERROR;
}

uint8 OBD_GetVehicleSpeed(uint8 *speed)
{
    return OBD_Request(OBD_PID_SPEED, speed, 1);
}

uint8 OBD_GetCoolantTemp(sint8 *temp)
{
    uint8 val;
    if(OBD_Request(OBD_PID_COOLANT, &val, 1) == OBD_STATUS_OK)
    {
        *temp = (sint8)val - 40;
        return OBD_STATUS_OK;
    }
    return OBD_STATUS_ERROR;
}

uint8 OBD_GetBatteryVoltage(float32 *volt)
{
    uint8 data[2];
    if(OBD_Request(OBD_PID_VOLTAGE, data, 2) == OBD_STATUS_OK)
    {
        *volt = ((uint16)data[0] * 256 + data[1]) / 1000.0f;
        return OBD_STATUS_OK;
    }
    return OBD_STATUS_ERROR;
}
