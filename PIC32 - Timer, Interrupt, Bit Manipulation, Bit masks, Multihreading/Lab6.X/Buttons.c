/*
 * File:   Buttons.c
 * Author: Brian
 * No collaboration 
 *
 * Created on May 16, 2017, 10:09 PM
 */

//CMPE13 Support Library
#include "BOARD.h"
#include "Buttons.h"

// **** Define global, module-level, or external variables here ****
uint8_t debounce[4];
uint8_t present = 0x00, cValue = 0x00, tempT = 0x00;

/**
 * This function initializes the proper pins such that the buttons 1-4 may be used by modifying
 * the necessary bits in TRISD/TRISF. Only the bits necessary to enable the 1-4 buttons are
 * modified so that this library does not interfere with other libraries.
 */
void ButtonsInit(void)
{
    TRISD |= 0x00E0;
    TRISF |= 0x0002;
}

/**
 * This function checks the button states and returns any events that have occured since the last
 * call. In the case of the first call to ButtonsCheckEvents() after ButtonsInit(), the function
 * should assume that the buttons start in an off state with value 0. Therefore if no buttons are
 * pressed when ButtonsCheckEvents() is first called, BUTTONS_EVENT_NONE should be returned. The
 * events are listed in the ButtonEventFlags enum at the top of this file. This function should be
 * called repeatedly.
 *
 * This function also performs debouncing of the buttons. Every time ButtonsCheckEvents() is called,
 * all button values are saved, up to the 4th sample in the past, so 4 past samples and the present
 * values. A button event is triggered if the newest 4 samples are the same and different from the
 * oldest sample. For example, if button 1 was originally down, button 1 must appear to be up for 4
 * samples and the last BTN3 event was BUTTON_EVENT_3DOWN before a BUTTON_EVENT_1UP is triggered. 
 * This eliminates button bounce, where the button may quickly oscillate between the ON and OFF state
 * as it's being pressed or released.
 *
 * NOTE: This will not work properly without ButtonsInit() being called beforehand.
 * @return A bitwise-ORing of the constants in the ButtonEventFlags enum or BUTTON_EVENT_NONE if no
 *         event has occurred.
 */
uint8_t ButtonsCheckEvents(void)
{
    // Initialize
    int i = 0;
    int j = 0;
    tempT = 0x00;
    cValue = BUTTON_STATES();


    /* 
     * Check bitwise
     * Check button 1
     */
    if (((debounce[0] & 0x01) == (cValue & 0x01)) && ((debounce[1] & 0x01) == (cValue & 0x01)) && ((debounce[2]&0x01) == (cValue & 0x01)) && ((debounce[3] & 0x01) != (cValue & 0x01))) {
        if (debounce[3] & 0x01) {
            tempT |= BUTTON_EVENT_1UP;
        } else {
            tempT |= BUTTON_EVENT_1DOWN;
        }
        j = 1;
    }

    // Check button 2
    if (((debounce[0] & 0x02) == (cValue & 0x02)) && ((debounce[1] & 0x02) == (cValue & 0x02)) && ((debounce[2] & 0x02) == (cValue & 0x02)) && ((debounce[3] & 0x02) != (cValue & 0x02))) {
        if (debounce[3] & 0x02) {
            tempT |= BUTTON_EVENT_2UP;
        } else {
            tempT |= BUTTON_EVENT_2DOWN;
        }
        j = 1;
    }

    // Check button 3
    if (((debounce[0] & 0x04) == (cValue & 0x04)) && ((debounce[1] & 0x04) == (cValue & 0x04)) && ((debounce[2] & 0x04) == (cValue & 0x04)) && ((debounce[3] & 0x04) != (cValue & 0x04))) {
        if (debounce[3] &0x04) {
            tempT |= BUTTON_EVENT_3UP;
        } else {
            tempT |= BUTTON_EVENT_3DOWN;
        }
        j = 1;
    }

    // Check button 4
    if (((debounce[0] & 0x08) == (cValue & 0x08)) && ((debounce[1] & 0x08) == (cValue & 0x08)) && ((debounce[2] & 0x08) == (cValue & 0x08)) && ((debounce[3] & 0x08) != (cValue & 0x08))) {
        if (debounce[3] &0x08) {
            tempT |= BUTTON_EVENT_4UP;
        } else {
            tempT |= BUTTON_EVENT_4DOWN;
        }
        j = 1;
    }

    // Save previous calls
    for (i = 4; i > 0; i--) {
        if (i - 1 == 0) {
            debounce[i - 1] = cValue;
        } else {
            debounce[i - 1] = debounce[i - 2];
        }
    }

    // Determine what will be return
    if (j == 0) {
        return BUTTON_EVENT_NONE;
    } else {
        return tempT;
    }
}
