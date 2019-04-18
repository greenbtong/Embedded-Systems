// **** Include libraries here ****
// Standard libraries

//CMPE13 Support Library
#include "BOARD.h"
#include "Oled.h"
#include "OledDriver.h"
#include "Leds.h"
#include "Ascii.h"
#include "Adc.h"
#include "Buttons.h"

// Microchip libraries
#include <xc.h>
#include <plib.h>
#include <stdio.h>


// **** Set any macros or preprocessor directives here ****
// Set a macro for resetting the timer, makes the code a little clearer.
#define TIMER_2HZ_RESET() (TMR1 = 0)

#define BAKE_MODE 0
#define TOAST_MODE 1
#define BROIL_MODE 2
#define TIMER_SEL 0
#define TEMP_SEL 1
#define OVEN_OFF 0 
#define OVEN_ON 1
#define LONG_PRESS 5
#define ZERO 0

// **** Declare any datatypes here ****

// **** Define any module-level, global, or external variables here ****

enum {
    RESET,
    START,
    COUNTDOWN,
    PENDING_SELECTOR_CHANGE,
    PENDING_RESET,
    INVERT, 
    NORMAL
} oState = RESET;

typedef struct Oven {
    int cookingTL;
    int cookingT;
    int temp;
    int cookingM;
    int ovenS;
    uint8_t btnPress;
    int inputSel;
} Oven;

void displayOLED();

static Oven ovn;
static char output[100]; // output
static int adcFlag = 0; // Adc event/flag
static int btnFlag = 0; // counter for button 
static int adcValue = 0; // value of Adc
static int flag = 0; // time counter/flag
static int freeCounter = 0; // holding down counter
static int i = 0; // counters
static int j = 2; // counters
static uint8_t btnEvents = 0x00; // button event


// Configuration Bit settings

int main()
{
    BOARD_Init();

    // Configure Timer 1 using PBCLK as input. We configure it using a 1:256 prescalar, so each timer
    // tick is actually at F_PB / 256 Hz, so setting PR1 to F_PB / 256 / 2 yields a 0.5s timer.
    OpenTimer1(T1_ON | T1_SOURCE_INT | T1_PS_1_256, BOARD_GetPBClock() / 256 / 2);

    // Set up the timer interrupt with a medium priority of 4.
    INTClearFlag(INT_T1);
    INTSetVectorPriority(INT_TIMER_1_VECTOR, INT_PRIORITY_LEVEL_4);
    INTSetVectorSubPriority(INT_TIMER_1_VECTOR, INT_SUB_PRIORITY_LEVEL_0);
    INTEnable(INT_T1, INT_ENABLED);

    // Configure Timer 2 using PBCLK as input. We configure it using a 1:16 prescalar, so each timer
    // tick is actually at F_PB / 16 Hz, so setting PR2 to F_PB / 16 / 100 yields a .01s timer.
    OpenTimer2(T2_ON | T2_SOURCE_INT | T2_PS_1_16, BOARD_GetPBClock() / 16 / 100);

    // Set up the timer interrupt with a medium priority of 4.
    INTClearFlag(INT_T2);
    INTSetVectorPriority(INT_TIMER_2_VECTOR, INT_PRIORITY_LEVEL_4);
    INTSetVectorSubPriority(INT_TIMER_2_VECTOR, INT_SUB_PRIORITY_LEVEL_0);
    INTEnable(INT_T2, INT_ENABLED);

    // Configure Timer 3 using PBCLK as input. We configure it using a 1:256 prescalar, so each timer
    // tick is actually at F_PB / 256 Hz, so setting PR3 to F_PB / 256 / 5 yields a .2s timer.
    OpenTimer3(T3_ON | T3_SOURCE_INT | T3_PS_1_256, BOARD_GetPBClock() / 256 / 5);

    // Set up the timer interrupt with a medium priority of 4.
    INTClearFlag(INT_T3);
    INTSetVectorPriority(INT_TIMER_3_VECTOR, INT_PRIORITY_LEVEL_4);
    INTSetVectorSubPriority(INT_TIMER_3_VECTOR, INT_SUB_PRIORITY_LEVEL_0);
    INTEnable(INT_T3, INT_ENABLED);

    /***************************************************************************************************
     * Your code goes in between this comment and the following one with asterisks.
     **************************************************************************************************/

    // Initialize
    LEDS_INIT();
    OledInit();
    AdcInit();
    int LEDsplit;
    ovn.cookingT = 1;
    ovn.inputSel = TIMER_SEL;
    ovn.cookingM = BAKE_MODE;
    ovn.temp = 350;
    flag = FALSE;


    while (1) {
        switch (oState) {

        case RESET: // Default Case
            ovn.cookingTL = ovn.cookingT;
            ovn.ovenS = OVEN_OFF;
            ovn.btnPress = 0x00;
            LEDS_SET(0x00);
            displayOLED();
            if (flag) {
                oState = START;
            }
            break;

        case START: // Continue to update until oven is on
            displayOLED();

            // ADC reading 
            if (adcFlag) {
                if (ovn.inputSel == TIMER_SEL) {
                    ovn.cookingT = (int) (adcValue >> 2) + 1;
                } else if (ovn.inputSel == TEMP_SEL) {
                    ovn.temp = (int) (((adcValue >> 2) + 300));
                }
                ovn.cookingTL = ovn.cookingT;
                displayOLED();
                adcFlag = 0;
            }

            // Check button event
            if (btnEvents) {
                oState = PENDING_SELECTOR_CHANGE;
            } else {
                oState = START;
            }
            break;


        case PENDING_SELECTOR_CHANGE: // Checks what the user wants to do
            displayOLED();

            // Determine what button three does
            if (btnEvents == BUTTON_EVENT_3DOWN) {
                freeCounter = ZERO;
                btnFlag = 1;
                ovn.btnPress = BUTTON_EVENT_3DOWN;
            }
            while (btnFlag) {
                // Switch between modes
                if (btnEvents == BUTTON_EVENT_3UP) {
                    if ((ovn.btnPress == BUTTON_EVENT_3DOWN) && (ovn.cookingM == BAKE_MODE) && (freeCounter < LONG_PRESS)) {
                        ovn.cookingM = TOAST_MODE;
                        ovn.inputSel = TIMER_SEL;
                        ovn.temp = 350;
                        ovn.cookingT = 1;
                        displayOLED();
                    } else if ((ovn.btnPress == BUTTON_EVENT_3DOWN) && (ovn.cookingM == TOAST_MODE) && (freeCounter < LONG_PRESS)) {
                        ovn.cookingM = BROIL_MODE;
                        ovn.inputSel = TIMER_SEL;
                        ovn.temp = 350;
                        ovn.cookingT = 1;
                        displayOLED();
                    } else if ((ovn.btnPress == BUTTON_EVENT_3DOWN) && (ovn.cookingM == BROIL_MODE) && (freeCounter < LONG_PRESS)) {
                        ovn.cookingM = BAKE_MODE;
                        ovn.inputSel = TIMER_SEL;
                        ovn.temp = 350;
                        ovn.cookingT = 1;
                        displayOLED();
                    }
                    btnFlag = 0;
                }

                // Holding down
                if ((btnFlag == 1) && (ovn.btnPress == BUTTON_EVENT_3DOWN) && (ovn.cookingM == BAKE_MODE) && (freeCounter >= LONG_PRESS)) {
                    if (ovn.inputSel == TIMER_SEL) {
                        ovn.inputSel = TEMP_SEL;
                        btnFlag = 0;
                        displayOLED();
                    } else {
                        ovn.inputSel = TIMER_SEL;
                        btnFlag = 0;
                        displayOLED();
                    }
                }
            }

            // if button 4 is pressed everything goes through
            if (btnEvents == BUTTON_EVENT_4DOWN) {
                btnFlag = 0;
                ovn.btnPress = 0x00;
                ovn.inputSel = TIMER_SEL;
                ovn.ovenS = OVEN_ON;
                displayOLED();
                oState = COUNTDOWN;
            } else {
                ovn.btnPress = 0x00;
                oState = START;
            }
            break;

        case COUNTDOWN: // LED countdown
            displayOLED();

            // Set up LED
            if (ovn.cookingT <= 8) {
                LEDsplit = (int) (ovn.cookingTL * 8 / ovn.cookingT);
            } else {
                LEDsplit = (int) ((ovn.cookingTL + 1) * 8 / ovn.cookingT);
            }

            if (LEDsplit == 8) {
                LEDS_SET(0xFF);
            } else if (LEDsplit == 7) {
                LEDS_SET(0xFE);
            } else if (LEDsplit == 6) {
                LEDS_SET(0xFC);
            } else if (LEDsplit == 5) {
                LEDS_SET(0xF8);
            } else if (LEDsplit == 4) {
                LEDS_SET(0xF0);
            } else if (LEDsplit == 3) {
                LEDS_SET(0xE0);
            } else if (LEDsplit == 2) {
                LEDS_SET(0xC0);
            } else if (LEDsplit == 1) {
                LEDS_SET(0x80);
            } else if (LEDsplit == 0) {
                LEDS_SET(0x00);
            }

            // Timer countdown
            if ((flag == TRUE) && (ovn.cookingTL > 0)) {
                flag = FALSE;
                i++;
                if (i == 2) {
                    ovn.cookingTL -= 1;
                    i = FALSE;
                }
                oState = COUNTDOWN;
            } else if (ovn.cookingTL == 0) {
                OledSetDisplayInverted();
                ovn.btnPress = 0x00;
                oState = NORMAL;
            }

            // If button 4 is pressed
            if (btnEvents == BUTTON_EVENT_4DOWN) {
                freeCounter = ZERO;
                btnFlag = 1;
                ovn.btnPress = BUTTON_EVENT_4DOWN;
                oState = PENDING_RESET;
            }
            break;



        case PENDING_RESET: // Check if user wants to restart
            while (btnFlag) {
                // Switch between modes
                if (btnEvents == BUTTON_EVENT_4UP) {
                    if ((ovn.btnPress == BUTTON_EVENT_4DOWN) && (freeCounter < LONG_PRESS)) {
                        btnFlag = 0;
                        displayOLED();
                        ovn.btnPress = 0x00;
                        oState = COUNTDOWN;
                    }

                }

                // Holding down
                if ((btnFlag == 1) && (ovn.btnPress == BUTTON_EVENT_4DOWN) && (ovn.ovenS == OVEN_ON) && (freeCounter >= LONG_PRESS)) {
                    btnFlag = 0;
                    oState = RESET;
                }
            }
            break;

        case INVERT: // Flash the screen
            // Counter
            if (flag) {
                flag = FALSE;
                j--;
                oState = INVERT;
                if (j == 0) {
                    OledSetDisplayInverted();
                    oState = NORMAL;
                    j = 2;
                }
            }

            // If button 4 is press exit back to start
            if (btnEvents == BUTTON_EVENT_4DOWN) {
                OledSetDisplayNormal();
                flag = FALSE;
                oState = RESET;
            }
            break;


        case NORMAL: // Unflash the screen
            // Counter
            if (flag) {
                flag = FALSE;
                j--;
                oState = NORMAL;
                if (j == 0) {
                    OledSetDisplayNormal();
                    oState = INVERT;
                    j = 2;
                }
            }

            // If button 4 is press exit back to start
            if (btnEvents == BUTTON_EVENT_4DOWN) {
                OledSetDisplayNormal();
                flag = FALSE;
                oState = RESET;
            }
            break;

        }
    }
    /***************************************************************************************************
     * Your code goes in between this comment and the preceding one with asterisks
     **************************************************************************************************/
    while (1);
}

void __ISR(_TIMER_1_VECTOR, ipl4auto) TimerInterrupt2Hz(void)
{
    // Clear the interrupt flag.
    IFS0CLR = 1 << 4;
    flag = TRUE;
}

void __ISR(_TIMER_3_VECTOR, ipl4auto) TimerInterrupt5Hz(void)
{
    // Clear the interrupt flag.
    IFS0CLR = 1 << 12;
    freeCounter++;
}

void __ISR(_TIMER_2_VECTOR, ipl4auto) TimerInterrupt100Hz(void)
{
    // Clear the interrupt flag.
    IFS0CLR = 1 << 8;

    // Check for button events
    btnEvents = ButtonsCheckEvents();

    // Check ADC
    if (AdcChanged()) {
        adcValue = AdcRead();
        adcFlag = 1;
    }
}

// Output function

void displayOLED()
{
    char *mode;
    char pick1 = '>';
    char pick2 = ' ';
    char tempOutput1[15], tempOutput3[15], tempOutput4[15];

    // Determine the mode
    sprintf(tempOutput1, "%c%c%c%c%c%c%s", '|', 0x02, 0x02, 0x02, 0x02, 0x02, "|  Mode: ");
    sprintf(tempOutput4, "%c%c%c%c%c%c%c", '|', 0x04, 0x04, 0x04, 0x04, 0x04, '|');
    if (ovn.cookingM == BAKE_MODE) {
        mode = "Bake ";
        if (ovn.inputSel == TIMER_SEL) {
            pick1 = '>';
            pick2 = ' ';
        } else if (ovn.inputSel == TEMP_SEL) {
            pick1 = ' ';
            pick2 = '>';
        }
        if (ovn.ovenS == OVEN_ON) {
            sprintf(tempOutput1, "%c%c%c%c%c%c%s", '|', 0x01, 0x01, 0x01, 0x01, 0x01, "|  Mode: ");
            sprintf(tempOutput4, "%c%c%c%c%c%c%c", '|', 0x03, 0x03, 0x03, 0x03, 0x03, '|');
            pick1 = ' ';
            pick2 = ' ';
        }
        sprintf(tempOutput3, "%c%s%d%c%c", pick2, "Temp: ", ovn.temp, 0xF8, 'F');
    } else if (ovn.cookingM == TOAST_MODE) {
        mode = "Toast ";
        pick1 = ' ';
        if (ovn.ovenS == OVEN_ON) {
            sprintf(tempOutput4, "%c%c%c%c%c%c%c", '|', 0x03, 0x03, 0x03, 0x03, 0x03, '|');
        }
        sprintf(tempOutput3, "%s", "            ");
    } else if (ovn.cookingM == BROIL_MODE) {
        mode = "Broil ";
        pick1 = ' ';
        if (ovn.ovenS == OVEN_ON) {
            sprintf(tempOutput1, "%c%c%c%c%c%c%s", '|', 0x01, 0x01, 0x01, 0x01, 0x01, "|  Mode: ");
        }
        sprintf(tempOutput3, "%c%s%d%c%c", pick2, "Temp: ", 500, 0xF8, 'F');
    }

    // Output
    sprintf(output, "%s%s\n%s%c%s%-d%c%02d%s\n%s%s\n%s", tempOutput1, mode,
            "|     | ", pick1, "Time: ", (int) ovn.cookingTL / 60, ':', (int) ovn.cookingTL % 60, " ",
            "|-----| ", tempOutput3,
            tempOutput4);
    OledDrawString(output);
    OledUpdate();
}