// **** Include libraries here ****
// Standard libraries

//CMPE13 Support Library
#include "BOARD.h"
#include "Protocol.h"
#include "Leds.h"
#include "Oled.h"
#include "Buttons.h"
#include "Protocol.h"
#include "OledDriver.h"
#include "Agent.h"
#include "Field.h"

// Microchip libraries
#include <xc.h>
#include <plib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

// User libraries
#include "Buttons.h"
#include "Oled.h"

// **** Set any macros or preprocessor directives here ****

// **** Declare any data types here ****

// **** Define any module-level, global, or external variables here ****
static uint32_t counter;
static uint8_t buttonEvents;
int i, j;
int c1, c2, c3, c4, ct;

// **** Declare any function prototypes here ****

int main()
{
    BOARD_Init();

    // Configure Timer 2 using PBCLK as input. We configure it using a 1:16 prescalar, so each timer
    // tick is actually at F_PB / 16 Hz, so setting PR2 to F_PB / 16 / 100 yields a 10ms timer.
    OpenTimer2(T2_ON | T2_SOURCE_INT | T2_PS_1_16, BOARD_GetPBClock() / 16 / 100);

    // Set up the timer interrupt with a medium priority of 4.
    INTClearFlag(INT_T2);
    INTSetVectorPriority(INT_TIMER_2_VECTOR, INT_PRIORITY_LEVEL_4);
    INTSetVectorSubPriority(INT_TIMER_2_VECTOR, INT_SUB_PRIORITY_LEVEL_0);
    INTEnable(INT_T2, INT_ENABLED);

    // Disable buffering on stdout
    setbuf(stdout, NULL);

    ButtonsInit();

    OledInit();

    // Prompt the user to start the game and block until the first character press.
    OledDrawString("Press BTN4 to start.");
    OledUpdate();

    char seed1[] = __TIME__;
    int seed1_len = strlen(seed1);
    int firstHalf = seed1_len / 2;
    uint16_t seed2 = 0;
    int i;
    for (i = 0; i < seed1_len; ++i) {
        seed2 ^= seed1[i] << ((i < firstHalf) ? 0 : 8);
    }
    srand(seed2 ^ ((uint16_t) (counter >> 16)) ^ (uint16_t) (counter));

    while ((buttonEvents & BUTTON_EVENT_4UP) == 0);


    /******************************************************************************
     * Your code goes in between this comment and the following one with asterisks.
     *****************************************************************************/
    while (1) {
        while ((buttonEvents & BUTTON_EVENT_4UP) == 0);
        printf("testing\n");
        NegotiationData nData, nData1;
        GuessData gData, gData1;
        //char *m =  "$COO,0,2*41";
        //char m[40];
        char *m = "$DET,52343,16067*50";
        char *m1 = "$DET,52343,16067*50";
        //int t;

        //ProtocolGenerateNegotiationData(&nData);
        //printf("guess: %x, enK: %x ,encrypg: %x, hash: %x\n", nData.guess, nData.encryptionKey, nData.encryptedGuess, nData.hash);
        //ProtocolEncodeCooMessage(m, &gData);
        //ProtocolEncodeChaMessage(m, &nData);
        //            for (t = 0; t < strlen(m); t++) {
        //                printf("%c", m[t]);
        //            }
        //            printf("\n\n");
        //printf("length %d\n", a);

        //int j = 0;
        for (i = 0; i <= strlen(m); i++) {

            ProtocolDecode(m[i], &nData, &gData);
        }
        for (i = 0; i <= strlen(m); i++) {

            ProtocolDecode(m1[i], &nData1, &gData1);
        }
        printf("%d\n", ProtocolGetTurnOrder(&nData, &nData1));
        //        
        //        printf("%d, %d \n", nData.guess, nData.encryptionKey);

    }
        //FieldPosition p; = FIELD_POSITION_EMPTY;
        // Field f;

        //    while (1) {
        //        while ((buttonEvents & BUTTON_EVENT_4UP) == 0);
        //        AgentInit();
        //        //        FieldInit(&f, FIELD_POSITION_EMPTY);
        //        //        ct = 1;
        //        //        c1 = 0;
        //        //        c2 = 0;
        //        //        c3 = 0;
        //        //        c4 = 0;
        //        //
        //        //        while (ct) {
        //        //            if (c1 == 0) {
        //        //                if (FieldAddBoat(&f, rand() % 6, rand() % 10, rand() % 4, FIELD_BOAT_SMALL)) {
        //        //                    c1 = 1;
        //        //                }
        //        //
        //        //            }
        //        //            if (c2 == 0) {
        //        //                if (FieldAddBoat(&f, rand() % 6, rand() % 10, rand() % 4, FIELD_BOAT_MEDIUM)) {
        //        //                    c2 = 1;
        //        //                }
        //        //            }
        //        //            if (c3 == 0) {
        //        //                if (FieldAddBoat(&f, rand() % 6, rand() % 10, rand() % 4, FIELD_BOAT_LARGE)) {
        //        //                    c3 = 1;
        //        //                }
        //        //            }
        //        //            if (c4 == 0) {
        //        //                if (FieldAddBoat(&f, rand() % 6, rand() % 10, rand() % 4, FIELD_BOAT_HUGE)) {
        //        //                    c4 = 1;
        //        //                }
        //        //            }
        //        //            if (c1 == 1 && c2 == 1 && c3 == 1 && c4 == 1) {
        //        //                ct = 0;
        //        //            }
        //        //        }
        //        //        for (i = 0; i < 6; i++) {
        //        //            for (j = 0; j < 10; j++) {
        //        //                printf("%d ", f.field[i][j]);
        //        //            }
        //        //            printf("\n");
        //        //        }
        //        //        printf("\n");
        //    }

        /******************************************************************************
         * Your code goes in between this comment and the preceeding one with asterisks
         *****************************************************************************/

        while (1);
    }

    /**
     * This is the interrupt for the Timer2 peripheral. It just keeps incrementing a counter used to
     * track the time until the first user input.
     */
    void __ISR(_TIMER_2_VECTOR, IPL4AUTO) TimerInterrupt100Hz(void)
    {
        // Clear the interrupt flag.
        IFS0CLR = 1 << 8;

        // Increment a counter to see the srand() function.
        counter++;

        // Also check for any button events
        buttonEvents = ButtonsCheckEvents();
    }