---

### **TM4C123GH6PM OBD-II CAN Driver Stack**

This repository contains a layered driver implementation for reading real-time vehicle diagnostics via the OBD-II port using a Tiva C Series (TM4C123GH6PM) microcontroller and a CAN transceiver (e.g., TJA1050).

---

### **1. Hardware Architecture**

The system is designed to interface with a standard OBD-II connector (J1962).

| **Pin** | **Function** | **Description** |
| --- | --- | --- |
| **PE4** | **CAN0 Rx** | Connect to CAN Transceiver **RXD** pin. |
| **PE5** | **CAN0 Tx** | Connect to CAN Transceiver **TXD** pin. |
| **GND** | **Ground** | Common ground with the vehicle/transceiver. |

* **Transceiver:** A 3.3V compatible CAN transceiver (like the TJA1050) is required to convert the microcontroller's logic levels to the differential CANH/CANL signals used by the vehicle.
* **Clock:** The driver assumes a **16 MHz** system clock.

---

### **2. Software Stack Architecture**

The software is organized into three distinct layers to ensure modularity and ease of use.

#### **Layer 1: Hardware Abstraction (HAL)**

* **Files:** `tm4c123gh6pm_registers.h`, `std_types.h`
* **Purpose:** Defines the base memory addresses for the microcontroller peripherals and standard data types (uint8, uint16, etc.).
* **Role:** Provides the raw pointers used by the upper layers to access hardware.

#### **Layer 2: CAN Driver (Data Link Layer)**

* **Files:** `can.c`, `can.h`
* **Purpose:** Manages the low-level transmission and reception of CAN frames.
* **Key Features:**
* **Bit Timing:** Configured for **500 kbps** (standard OBD-II speed) based on a 16 MHz clock.
* **Message Objects:** Uses Object 1 for transmission (Tx) and Object 2 for reception (Rx).
* **Filtering:** Supports hardware filtering to ignore non-OBD traffic.
* **Polling Mode:** Uses blocking functions with timeouts to ensure deterministic behavior without complex interrupt handling.



#### **Layer 3: OBD-II Driver (Application Layer)**

* **Files:** `obd.c`, `obd.h`
* **Purpose:** Implements the SAE J1979 protocol commands.
* **Key Features:**
* **Initialization:** Sets up the CAN layer and configures filters to accept ECU responses (IDs `0x7E8` – `0x7EF`).
* **Data Parsing:** Converts raw byte data from the vehicle into human-readable values (e.g., converting hex data to RPM or Temperature).
* **DTC Handling:** Includes support for reading and clearing Diagnostic Trouble Codes.



---

### **3. File Descriptions**

#### **`can.c` / `can.h**`

* **`CAN_Init`**: Enables Port E clocks, configures pins PE4/PE5, and sets the bit timing registers.
* **`CAN_Transmit`**: Loads a message into Interface Register 1 (IF1) and requests transmission via Message Object 1.
* **`CAN_Receive`**: Polls the "New Data" register. If data arrives, it loads it via Interface Register 2 (IF2) and clears the pending flag.
* **`CAN_ConfigureRxFilter`**: Sets the acceptance mask on Message Object 2 so the hardware only accepts relevant diagnostic response IDs.

#### **`obd.c` / `obd.h**`

* **`OBD_Init`**: Initializes the CAN driver and applies the filter mask `0x7F8` to accept standard OBD-II response IDs.
* **`OBD_Request`**: A helper function that formats a Service Mode 01 request, sends it, waits for a reply, and validates the response.
* **`OBD_Get[Sensor]`**: Specialized functions (e.g., `OBD_GetEngineRPM`) that encapsulate the specific PID and math formula for that sensor.

---

### **4. Usage Example**

To display real-time data, initialize the system once and then loop through the desired sensor functions.

```c
#include "obd.h"
#include "delay.h"

int main(void) {
    uint16 rpm;
    uint8 speed;

    /* Initialize OBD/CAN System */
    if (OBD_Init() != OBD_STATUS_OK) {
        /* Handle Error (e.g., turn on LED) */
        while(1);
    }

    while(1) {
        /* Read RPM */
        if (OBD_GetEngineRPM(&rpm) == OBD_STATUS_OK) {
            /* Display RPM on LCD */
        }

        /* Read Speed */
        if (OBD_GetVehicleSpeed(&speed) == OBD_STATUS_OK) {
            /* Display Speed on LCD */
        }

        /* Essential delay to prevent bus flooding */
        Delay_MS(50); 
    }
}

```

---

### **5. Configuration Notes**

* **Clock Speed:** The driver is hardcoded for a **16 MHz** system clock. If you use the PLL to increase the clock speed (e.g., to 80 MHz), you must recalculate the bit timing values in `CAN_SetBitTiming` inside `can.c`.
* **Timeouts:** The default timeout is set to **100ms** in `obd.h` (`OBD_RESPONSE_TIMEOUT`). This is sufficient for most vehicles, but can be increased if data is missed.
