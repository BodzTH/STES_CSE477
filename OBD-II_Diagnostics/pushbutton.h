#ifndef PUSHBUTTON_H
#define PUSHBUTTON_H

#include "std_types.h"

/* Button Functions for SW1 only */
void Button_Init(void);
boolean Button_IsPressed(void);
boolean Button_WasPressed(void);

/* Simple Macros for SW1 */
#define SW1_PRESSED()     Button_IsPressed()
#define SW1_WAS_PRESSED() Button_WasPressed()

#endif /* PUSHBUTTON_H */
