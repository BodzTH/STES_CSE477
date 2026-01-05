#include "tm4c123gh6pm_registers.h"
#include "std_types.h"
#include "lcd.h"
#include "obd.h"
#include "pushbutton.h"
#include "led.h"
#include "delay.h"

/* Application States */
typedef enum {
    STATE_SHOW_RPM,
    STATE_SHOW_SPEED,
    STATE_SHOW_COOLANT,
    STATE_SHOW_VOLTAGE,
    STATE_MAX_STATES
} AppState_t;

/* --- LCD Helper Functions --- */
void LCD_PrintInt(sint32 num)
{
    char buffer[12];
    char *ptr = buffer;
    uint32 u_val;
    
    if (num < 0) {
        LCD_SendData('-');
        u_val = (uint32)(-num);
    } else {
        u_val = (uint32)num;
    }
    
    if (u_val == 0) {
        LCD_SendData('0');
        return;
    }
    
    while (u_val > 0) {
        *ptr++ = (char)((u_val % 10) + '0');
        u_val /= 10;
    }
    
    while (ptr > buffer) {
        LCD_SendData(*(--ptr));
    }
}

void LCD_PrintFloat(float32 num)
{
    sint32 intPart = (sint32)num;
    sint32 decPart = (sint32)((num - intPart) * 10);
    if(decPart < 0) decPart = -decPart;
    LCD_PrintInt(intPart);
    LCD_SendData('.');
    LCD_PrintInt(decPart);
}

int main(void)
{
    AppState_t currentState = STATE_SHOW_RPM;
    uint8 status;
    boolean isConnected = FALSE;
    
    /* Data Variables */
    uint16 rpmVal = 0;
    uint8  speedVal = 0;
    sint8  tempVal = 0;
    float32 voltVal = 0.0f;
    
    /* Button Logic */
    boolean btnPrevState = FALSE;
    boolean btnCurrState = FALSE;
    
    /* 1. Hardware Initialization */
    LCD_Init();       
    Button_Init();    
    LED_Init();
    
    /* 2. Start-up Screen */
    LCD_Clear(); 
    LCD_SetCursor(0, 0);
    LCD_WriteString("OBD-II System");
    LCD_SetCursor(1, 0);
    LCD_WriteString("Initializing...");
    LED_On(LED_BLUE);
    Delay_MS(1000);
    
    /* 3. Connection Loop (Keeps trying until wiring is fixed) */
    while(!isConnected)
    {
        if(OBD_Init() == OBD_STATUS_OK)
        {
            isConnected = TRUE;
            LCD_Clear();
            LCD_SetCursor(0, 0);
            LCD_WriteString("OBD Connected!");
            LED_Off(LED_BLUE);
            LED_On(LED_GREEN);
            Delay_MS(1000);
        }
        else
        {
            /* If "FAIL 00" happens, we end up here */
            LCD_Clear();
            LCD_SetCursor(0, 0);
            LCD_WriteString("Connection Fail");
            LCD_SetCursor(1, 0);
            LCD_WriteString("Check Wiring...");
            
            /* Flash Red LED to alert user */
            LED_On(LED_RED);
            Delay_MS(500);
            LED_Off(LED_RED);
            Delay_MS(500);
            /* Loop repeats, trying to Init again */
        }
    }
    
    /* 4. Main Data Loop (Runs only after connection is successful) */
    LCD_Clear();
    LCD_SetCursor(0, 0);
    LCD_WriteString("Engine RPM:");
    
    while(1)
    {
        /* A. Check Button (Switch Screens) */
        btnCurrState = Button_IsPressed();
        if (btnCurrState == TRUE && btnPrevState == FALSE)
        {
            LED_On(LED_BLUE);
            currentState++;
            if(currentState >= STATE_MAX_STATES) currentState = STATE_SHOW_RPM;
            
            LCD_Clear();
            LCD_SetCursor(0, 0);
            switch(currentState)
            {
                case STATE_SHOW_RPM:     LCD_WriteString("Engine RPM:"); break;
                case STATE_SHOW_SPEED:   LCD_WriteString("Vehicle Speed:"); break;
                case STATE_SHOW_COOLANT: LCD_WriteString("Coolant Temp:"); break;
                case STATE_SHOW_VOLTAGE: LCD_WriteString("Battery Volt:"); break;
            }
        }
        else if (btnCurrState == FALSE && btnPrevState == TRUE)
        {
            LED_Off(LED_BLUE);
        }
        btnPrevState = btnCurrState;
        
        /* B. Fetch Data */
        LCD_SetCursor(1, 0);
        switch(currentState)
        {
            case STATE_SHOW_RPM:
                if(OBD_GetEngineRPM(&rpmVal) == OBD_STATUS_OK) {
                    LCD_PrintInt((sint32)rpmVal);
                    LCD_WriteString(" rpm     "); 
                } else LCD_WriteString("No Data...    ");
                break;
                
            case STATE_SHOW_SPEED:
                if(OBD_GetVehicleSpeed(&speedVal) == OBD_STATUS_OK) {
                    LCD_PrintInt((sint32)speedVal);
                    LCD_WriteString(" km/h    ");
                } else LCD_WriteString("No Data...    ");
                break;
                
            case STATE_SHOW_COOLANT:
                if(OBD_GetCoolantTemp(&tempVal) == OBD_STATUS_OK) {
                    LCD_PrintInt((sint32)tempVal);
                    LCD_WriteString(" C       ");
                } else LCD_WriteString("No Data...    ");
                break;
                
            case STATE_SHOW_VOLTAGE:
                if(OBD_GetBatteryVoltage(&voltVal) == OBD_STATUS_OK) {
                    LCD_PrintFloat(voltVal);
                    LCD_WriteString(" V       ");
                } else LCD_WriteString("No Data...    ");
                break;
        }
        
        /* C. Refresh Rate (100ms) */
        Delay_MS(100);
    }
}
