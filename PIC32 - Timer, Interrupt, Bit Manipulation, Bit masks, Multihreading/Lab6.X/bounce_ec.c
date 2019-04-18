// **** Include libraries here ****
// Standard libraries

//CMPE13 Support Library
#include "BOARD.h"
#include "Buttons.h"
#include "Leds.h"
#include "Oled.h"

// Microchip libraries
#include <xc.h>
#include <plib.h>

// User libraries


// **** Set macros and preprocessor directives ****

// **** Declare any datatypes here ****

typedef struct AdcResult {
    uint8_t event;
    uint16_t value;
    uint8_t event1;
    uint16_t value1;
} AdcResult;

// **** Define global, module-level, or external variables here ****
#define LEFT 1
#define RIGHT 0

uint8_t buttonEvents = 0x00;
AdcResult AdcData;

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



    // Enable interrupts for the ADC
    ConfigIntADC10(ADC_INT_PRI_2 | ADC_INT_SUB_PRI_0 | ADC_INT_ON);

    // Set B2 to an input so AN0 can be used by the ADC.
    TRISBCLR = 1 << 2;

    // Configure and start the ADC
    // Read AN0 as sample a. We don't use alternate sampling, so setting sampleb is pointless.
    SetChanADC10(ADC_CH0_NEG_SAMPLEA_NVREF | ADC_CH0_POS_SAMPLEA_AN2);
    OpenADC10(
            ADC_MODULE_ON | ADC_IDLE_CONTINUE | ADC_FORMAT_INTG16 | ADC_CLK_AUTO | ADC_AUTO_SAMPLING_ON,
            ADC_VREF_AVDD_AVSS | ADC_OFFSET_CAL_DISABLE | ADC_SCAN_OFF | ADC_SAMPLES_PER_INT_8 |
            ADC_BUF_16 | ADC_ALT_INPUT_OFF,
            ADC_SAMPLE_TIME_29 | ADC_CONV_CLK_PB | ADC_CONV_CLK_51Tcy2,
            ENABLE_AN2_ANA,
            SKIP_SCAN_ALL);
    EnableADC10();

    /***************************************************************************************************
     * Your code goes in between this comment and the following one with asterisks.
     **************************************************************************************************/

    // Initialize buttons library 
    LEDS_INIT();
    ButtonsInit();
    uint8_t tempLED;
    OledInit();
    AdcData.event = 0;
    AdcData.value = 0;
    AdcData.event1 = 0;
    AdcData.value1 = 0;
    char output[100];
    float percent;
    uint8_t light = 0x01, tempLight;
    int dir = 0;


    while (1) {

        /* 
         * part3
         * Event check
         */
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
            // tempLED = 0x00;
            buttonEvents = 0;
        }

        // Part 1
        tempLight = light | tempLED;
        LEDS_SET(tempLight);

        if (AdcData.event1 == 1) {

            // Direction of LED
            if (light == 0x80) {
                dir = LEFT;
            } else if (light == 0x01) {
                dir = RIGHT;
            }

            // Move to the next LED (by bits)
            if (dir == LEFT) {
                light = light >> 1;
            } else if (dir == RIGHT) {
                light = light << 1;
            }
        }

        tempLED = 0x00;
        AdcData.event1 = 0;

        // Part 2
        if (AdcData.event == 1) {

            // Calculate percentage 
            percent = AdcData.value * 100 / 1023;

            // Output on to display
            sprintf(output, "Potentiometer value:\n%4d \n%3d%%  ", AdcData.value, (int) percent);
            OledDrawString(output);
            OledUpdate();


        }
        AdcData.event = 0;

        // Part 1
        LEDS_SET(light);

        if (AdcData.event1 == 1) {

            // Direction of LED
            if (light == 0x80) {
                dir = LEFT;
            } else if (light == 0x01) {
                dir = RIGHT;
            }

            // Move to the next LED (by bits)
            if (dir == LEFT) {
                light = light >> 1;
            } else if (dir == RIGHT) {
                light = light << 1;
            }
        }

        AdcData.event1 = 0;

    }



    /***************************************************************************************************
     * Your code goes in between this comment and the preceding one with asterisks
     **************************************************************************************************/

    while (1);
}

/**
 * This is the interrupt for the Timer1 peripheral. It checks for button events and stores them in a
 * module-level variable. Additionally during each call it increments a counter (the value member of
 * a module-level TimerResult struct). This counter is then checked against the top four bits of the
 * ADC result, and if it's greater, then the event member of a module-level TimerResult struct is
 * set to true and the value member is cleared.
 */
void __ISR(_TIMER_1_VECTOR, IPL4AUTO) Timer1Handler(void)
{
    // Clear the interrupt flag.
    INTClearFlag(INT_T1);

    // Check for button events
    buttonEvents = ButtonsCheckEvents();

}

/**
 * This is the ISR for the ADC1 peripheral. It has been enabled to run continuously. Reads all 8
 * samples from the ADC, averages them, and stores them in a module-level variable for use in the
 * main event loop.
 */
void __ISR(_ADC_VECTOR, IPL2AUTO) AdcHandler(void)
{
    AdcData.value1++;

    // Clear the interrupt flag.
    INTClearFlag(INT_AD1);

    // Get average from all ADC registers (8)
    double avg = (double) (ADC1BUF0 + ADC1BUF1 + ADC1BUF2 + ADC1BUF3 + ADC1BUF4 + ADC1BUF5 + ADC1BUF6 + ADC1BUF7) / 8;

    // If changed between this run and the last one -> event = 1
    if (AdcData.value != avg) {
        AdcData.event = 1;
    }

    // Store average into a AdcResult struct
    AdcData.value = avg;

    // If current counter is greater than switch value
    if ((int) AdcData.value1 + AdcData.value * 5 / 341 > 20) {
        AdcData.event1 = 1;
        AdcData.value1 = 0;
    }


}