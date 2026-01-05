#ifndef OBD_H_
#define OBD_H_

#include "std_types.h"

/* OBD IDs */
#define OBD_REQUEST_ID          0x7DF
#define OBD_RESPONSE_ID_MIN     0x7E8
#define OBD_RESPONSE_ID_MAX     0x7EF

/* PIDs */
#define OBD_MODE_CURRENT        0x01
#define OBD_PID_RPM             0x0C
#define OBD_PID_SPEED           0x0D
#define OBD_PID_COOLANT         0x05
#define OBD_PID_VOLTAGE         0x42

/* Errors */
#define OBD_STATUS_OK           0
#define OBD_STATUS_ERROR        1
#define OBD_STATUS_TIMEOUT      2

/* Functions */
uint8 OBD_Init(void);
uint8 OBD_GetEngineRPM(uint16 *rpm);
uint8 OBD_GetVehicleSpeed(uint8 *speed);
uint8 OBD_GetCoolantTemp(sint8 *temp);
uint8 OBD_GetBatteryVoltage(float32 *volt);

#endif /* OBD_H_ */
