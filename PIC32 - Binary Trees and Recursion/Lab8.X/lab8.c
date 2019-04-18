// **** Include libraries here ****
// Standard C libraries

// HEAP size 1024

//CMPE13 Support Library
#include "BOARD.h"
#include "Morse.h"
#include "OledDriver.h"

// Microchip libraries
#include <xc.h>
#include <plib.h>
#include <string.h>

// User libraries


// **** Set any macros or preprocessor directives here ****
// Specify a bit mask for setting/clearing the pin corresponding to BTN4. Should only be used when
// unit testing the Morse event checker.
#define BUTTON4_STATE_FLAG (1 << 7)

// **** Declare any data types here ****

// **** Define any module-level, global, or external variables here ****
static char printOUT[100];
static char chars[15];
static char letters[50];
static char *passDown = " ";
static char c;
static MorseEvent val;
static MorseEvent i;
static int flag;


// **** Declare any function prototypes here ****
void appendBot();
void appendTop();
void clrUpdate();

int main()
{
    BOARD_Init();


    // Configure Timer 2 using PBCLK as input. We configure it using a 1:16 prescalar, so each timer
    // tick is actually at F_PB / 16 Hz, so setting PR2 to F_PB / 16 / 100 yields a .01s timer.
    OpenTimer2(T2_ON | T2_SOURCE_INT | T2_PS_1_16, BOARD_GetPBClock() / 16 / 100);

    // Set up the timer interrupt with a medium priority of 4.
    INTClearFlag(INT_T2);
    INTSetVectorPriority(INT_TIMER_2_VECTOR, INT_PRIORITY_LEVEL_4);
    INTSetVectorSubPriority(INT_TIMER_2_VECTOR, INT_SUB_PRIORITY_LEVEL_0);
    INTEnable(INT_T2, INT_ENABLED);

    /******************************************************************************
     * Your code goes in between this comment and the following one with asterisks.
     *****************************************************************************/

    // Initialize
    MorseInit();
    OledInit();

    while (1) {

        // If event
        if (flag) {

            if (i == MORSE_EVENT_DOT) {
                MorseDecode(MORSE_CHAR_DOT);
                passDown = ".";
                appendTop();
            } else if (i == MORSE_EVENT_DASH) {
                MorseDecode(MORSE_CHAR_DASH);
                passDown = "-";
                appendTop();
            } else if (i == MORSE_EVENT_INTER_LETTER) {
                c = MorseDecode(MORSE_CHAR_END_OF_CHAR);
                appendBot();
                clrUpdate();
            } else if (i == MORSE_EVENT_INTER_WORD) {
                MorseDecode(MORSE_CHAR_DECODE_RESET);
                c = ' ';
                appendBot();
                clrUpdate();
            }
            sprintf(printOUT, "%s\n%s", chars, letters);
            OledDrawString(printOUT);
            OledUpdate();
            flag = 0;

        }
    }

    /******************************************************************************
     * Your code goes in between this comment and the preceding one with asterisks.
     *****************************************************************************/

    while (1);
}

void __ISR(_TIMER_2_VECTOR, IPL4AUTO) TimerInterrupt100Hz(void)
{
    // Clear the interrupt flag.
    IFS0CLR = 1 << 8;

    //******** Put your code here *************//

    // morse EVENT
    val = MorseCheckEvents();
    if (val != MORSE_EVENT_NONE) {
        i = val;
        flag = 1;
    }

}

// appends a character to the top line

void appendTop()
{
    sprintf(chars, "%s%s", chars, passDown);
}

// appends a character to the bot line

void appendBot()
{
    sprintf(letters, "%s%c", letters, c);
}

// clears the top line of oled

void clrUpdate()
{
    memset(chars, 0, sizeof (chars));
    OledClear(0);
}