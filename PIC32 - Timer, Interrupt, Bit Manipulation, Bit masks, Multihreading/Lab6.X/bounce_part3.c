// **** Include libraries here ****
// Standard libraries

//CMPE13 Support Library
#include "BOARD.h"
#include "Leds.h"
#include "Buttons.h"

// Microchip libraries
#include <xc.h>
#include <plib.h>

// User libraries


// **** Set macros and preprocessor directives ****

// **** Declare any datatypes here ****

// **** Define global, module-level, or external variables here ****
uint8_t buttonEvents = 0x00;

// **** Declare function prototypes ****

int main(void)
{
    BOARD_Init();

    // Configure Timer 1 using PBCLK as input. This default period will make the LEDs blink at a
    // pretty reasonable rate to start.
    OpenTimer1(T1_ON | T1_SOURCE_INT | T1_PS_1_8, 0xFFFF);

    // Set up the timer interrupt with a priority of 4.
    INTClearFlag(INT_T1);
    INTSetVectorPriority(INT_TIMER_1_VECTOR, INT_PRIORITY_LEVEL_4);
    INTSetVectorSubPriority(INT_TIMER_1_VECTOR, INT_SUB_PRIORITY_LEVEL_0);
    INTEnable(INT_T1, INT_ENABLED);

    /***************************************************************************************************
     * Your code goes in between this comment and the following one with asterisks.
     **************************************************************************************************/

    // Initialize buttons library 
    LEDS_INIT();
    ButtonsInit();
    uint8_t tempLED;

    while (1) {
        // Event check
        buttonEvents = ButtonsCheckEvents();
        if (buttonEvents) {

            // Store current switch position
            tempLED = LATE;

            /* 
             * Check if switch 1 is on & button 1 is up 
             * Else switch 1 is off & button 1 is down
             */
            if (SW1 == 1 && (buttonEvents & BUTTON_EVENT_1UP) == BUTTON_EVENT_1UP) {
                /* 
                 * Check if 0000 0011 (if LED are on) 
                 * Else turn on 
                 */
                if ((tempLED & 0x03) == 0x03) {
                    tempLED &= 0xFC;
                } else {
                    tempLED += 0x03;
                }
            } else if (SW1 == 0 && (buttonEvents & BUTTON_EVENT_1DOWN) == BUTTON_EVENT_1DOWN) {
                if ((tempLED & 0x03) == 0x03) {
                    tempLED &= 0xFC;
                } else {
                    tempLED += 0x03;
                }
            }

            /* 
             * Check if switch 2 is on & button 2 is up 
             * Else switch 2 is off & button 2 is down
             */
            if (SW2 == 1 && (buttonEvents & BUTTON_EVENT_2UP) == BUTTON_EVENT_2UP) {
                /* 
                 * Check if 0000 1100 (if LED are on) 
                 * Else turn on 
                 */
                if ((tempLED & 0x0C) == 0x0C) {
                    tempLED &= 0xF3;
                } else {
                    tempLED += 0x0C;
                }
            } else if (SW2 == 0 && (buttonEvents & BUTTON_EVENT_2DOWN) == BUTTON_EVENT_2DOWN) {
                if ((tempLED & 0x0C) == 0x0C) {
                    tempLED &= 0xF3;
                } else {
                    tempLED += 0x0C;
                }
            }

            /* 
             * Check if switch 3 is on & button 3 is up 
             * Else switch 3 is off & button 3 is down
             */
            if (SW3 == 1 && (buttonEvents & BUTTON_EVENT_3UP) == BUTTON_EVENT_3UP) {
                /* 
                 * Check if 0011 0000 (if LED are on) 
                 * Else turn on 
                 */
                if ((tempLED & 0x30) == 0x30) {
                    tempLED &= 0xCF;
                } else {
                    tempLED += 0x30;
                }
            } else if (SW3 == 0 && (buttonEvents & BUTTON_EVENT_3DOWN) == BUTTON_EVENT_3DOWN) {
                if ((tempLED & 0x30) == 0x30) {
                    tempLED &= 0xCF;
                } else {
                    tempLED += 0x30;
                }
            }

            /* 
             * Check if switch 4 is on & button 4 is up 
             * Else switch 4 is off & button 4 is down
             */
            if (SW4 == 1 && (buttonEvents & BUTTON_EVENT_4UP) == BUTTON_EVENT_4UP) {
                /* 
                 * Check if 1100 0000 (if LED are on) 
                 * Else turn on 
                 */
                if ((tempLED & 0xC0) == 0xC0) {
                    tempLED &= 0x3F;
                } else {
                    tempLED += 0xC0;
                }
            } else if (SW4 == 0 && (buttonEvents & BUTTON_EVENT_4DOWN) == BUTTON_EVENT_4DOWN) {
                if ((tempLED & 0xC0) == 0xC0) {
                    tempLED &= 0x3F;
                } else {
                    tempLED += 0xC0;
                }
            }

            LEDS_SET(tempLED);
            tempLED = 0x00;
            buttonEvents = 0;
        }
    }

    /***************************************************************************************************
     * Your code goes in between this comment and the preceding one with asterisks
     **************************************************************************************************/

    while (1);
}

/**
 * This is the interrupt for the Timer1 peripheral. It checks for button events and stores them in a
 * module-level variable.
 */
void __ISR(_TIMER_1_VECTOR, IPL4AUTO) Timer1Handler(void)
{
    // Clear the interrupt flag.
    INTClearFlag(INT_T1);

    // Check for button events
    buttonEvents = ButtonsCheckEvents();
}