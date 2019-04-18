/*
 * File:   ArtificialAgent.c
 * Author: Brian Tong and Sam Hanawalt
 *
 * Created on June 7, 2017, 11:00 AM
 */

// Standard libraries
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// User libraries
#include "Protocol.h"
#include "Field.h"
#include "FieldOled.h"
#include "Oled.h"
#include "Agent.h"

#define TRUE 1
#define FALSE 0

#define ERROR_STRING_PARSING "Message parsing failed\n"
#define ERROR_STRING_NEG_DATA "Negotiation data is Invalid\n"
#define ERROR_STRING_ORDERING "Turn ordering was a tie\n"

static AgentState aState = AGENT_STATE_GENERATE_NEG_DATA;
static TurnOrder turn = FIELD_OLED_TURN_NONE;
static Field mField, tField;
static NegotiationData mData, tData;
static GuessData mGuessData, tGuessData;
static ProtocolParserStatus parseState;
static int c1, c2, c3, c4, ct;
static TurnOrder turnTemp;


// **** Define any module-level, global, or external variables here ****

/**
 * The Init() function for an Agent sets up everything necessary for an agent before the game
 * starts. This can include things like initialization of the field, placement of the boats,
 * etc. The agent can assume that stdlib's rand() function has been seeded properly in order to
 * use it safely within.
 */
void AgentInit(void)
{
    FieldInit(&mField, FIELD_POSITION_EMPTY);
    FieldInit(&tField, FIELD_POSITION_UNKNOWN);
    ct = 1;
    c1 = 0;
    c2 = 0;
    c3 = 0;
    c4 = 0;

    while (ct) {
        if (c1 == 0) {
            if (FieldAddBoat(&mField, rand() % 6, rand() % 10, rand() % 4, FIELD_BOAT_SMALL)) {
                c1 = 1;
            }
        }
        if (c2 == 0) {
            if (FieldAddBoat(&mField, rand() % 6, rand() % 10, rand() % 4, FIELD_BOAT_MEDIUM)) {
                c2 = 1;
            }
        }
        if (c3 == 0) {
            if (FieldAddBoat(&mField, rand() % 6, rand() % 10, rand() % 4, FIELD_BOAT_LARGE)) {
                c3 = 1;
            }
        }
        if (c4 == 0) {
            if (FieldAddBoat(&mField, rand() % 6, rand() % 10, rand() % 4, FIELD_BOAT_HUGE)) {
                c4 = 1;
            }
        }
        if (c1 == 1 && c2 == 1 && c3 == 1 && c4 == 1) {
            ct = 0;
        }
    }
    FieldOledDrawScreen(&mField, &tField, turn);
}

/**
 * The Run() function for an Agent takes in a single character. It then waits until enough
 * data is read that it can decode it as a full sentence via the Protocol interface. This data
 * is processed with any output returned via 'outBuffer', which is guaranteed to be 255
 * characters in length to allow for any valid NMEA0183 messages. The return value should be
 * the number of characters stored into 'outBuffer': so a 0 is both a perfectly valid output and
 * means a successful run.
 * @param in The next character in the incoming message stream.
 * @param outBuffer A string that should be transmit to the other agent. NULL if there is no
 *                  data.
 * @return The length of the string pointed to by outBuffer (excludes \0 character).
 */
int AgentRun(char in, char *outBuffer)
{
    if (in != NULL) {
        parseState = ProtocolDecode(in, &tData, &tGuessData);
    }
    switch (aState) {

    case AGENT_STATE_GENERATE_NEG_DATA:

        ProtocolGenerateNegotiationData(&mData);
        if (ProtocolValidateNegotiationData(&mData)) {
            aState = AGENT_STATE_SEND_CHALLENGE_DATA;
            ProtocolEncodeChaMessage(outBuffer, &mData);
            return strlen(outBuffer);
        }
        break;

    case AGENT_STATE_SEND_CHALLENGE_DATA:

        if (parseState == PROTOCOL_PARSED_CHA_MESSAGE) {
            printf("good");
            aState = AGENT_STATE_DETERMINE_TURN_ORDER;
            ProtocolEncodeDetMessage(outBuffer, &mData);
            return strlen(outBuffer);
        }
        //        else if (!parseState) {
        //            OledClear(OLED_COLOR_BLACK);
        //            OledDrawString(ERROR_STRING_PARSING);
        //            OledUpdate();
        //            aState = INVALID;
        //        }
        break;

    case AGENT_STATE_DETERMINE_TURN_ORDER:

        if (parseState == PROTOCOL_PARSED_DET_MESSAGE) {
            printf("check");
            if (ProtocolValidateNegotiationData(&tData)) {
                turnTemp = ProtocolGetTurnOrder(&mData, &tData);
                switch (turnTemp) {

                case TURN_ORDER_START:
                    turn = FIELD_OLED_TURN_MINE;
                    FieldOledDrawScreen(&mField, &tField, turn);
                    aState = AGENT_STATE_SEND_GUESS;
                    break;

                case TURN_ORDER_DEFER:
                    turn = FIELD_OLED_TURN_THEIRS;
                    FieldOledDrawScreen(&mField, &tField, turn);
                    aState = AGENT_STATE_WAIT_FOR_GUESS;
                    break;

                case TURN_ORDER_TIE:
                    OledClear(OLED_COLOR_BLACK);
                    OledDrawString(ERROR_STRING_ORDERING);
                    OledUpdate();
                    aState = AGENT_STATE_INVALID;
                    break;

                }
                //            } else {
                //                OledClear(OLED_COLOR_BLACK);
                //                OledDrawString(ERROR_STRING_NEG_DATA);
                //                OledUpdate();
                //                aState = AGENT_STATE_INVALID;
            }
        }

        break;

    case AGENT_STATE_SEND_GUESS:
        do {
            mGuessData.row = rand() % 6;
            mGuessData.col = rand() % 10;
        } while (FieldAt(&tField, mGuessData.row, mGuessData.col) != FIELD_POSITION_UNKNOWN);

        aState = AGENT_STATE_WAIT_FOR_HIT;
        ProtocolEncodeCooMessage(outBuffer, &mGuessData);
        return strlen(outBuffer);
        break;

    case AGENT_STATE_WAIT_FOR_HIT:

        if (parseState == PROTOCOL_PARSED_HIT_MESSAGE) {
            if (AgentGetEnemyStatus() == 0) {
                turn = FIELD_OLED_TURN_NONE;
                FieldOledDrawScreen(&mField, &tField, turn);
                aState = AGENT_STATE_WON;
            } else if (AgentGetEnemyStatus()) {
                FieldUpdateKnowledge(&tField, &tGuessData);
                turn = FIELD_OLED_TURN_THEIRS;
                FieldOledDrawScreen(&mField, &tField, turn);
                aState = AGENT_STATE_WAIT_FOR_GUESS;
            }
        }
        //        else if (!parseState) {
        //            OledClear(OLED_COLOR_BLACK);
        //            OledDrawString(ERROR_STRING_PARSING);
        //            OledUpdate();
        //            aState = INVALID;
        //        }
        break;

    case AGENT_STATE_WAIT_FOR_GUESS:

        if (parseState == PROTOCOL_PARSED_COO_MESSAGE) {
            if (AgentGetStatus() == 0) {
                turn = FIELD_OLED_TURN_NONE;
                FieldOledDrawScreen(&mField, &tField, turn);
                aState = AGENT_STATE_LOST;
            } else if (AgentGetStatus()) {
                FieldRegisterEnemyAttack(&mField, &tGuessData);
                turn = FIELD_OLED_TURN_MINE;
                FieldOledDrawScreen(&mField, &tField, turn);
                aState = AGENT_STATE_SEND_GUESS;
                ProtocolEncodeHitMessage(outBuffer, &tGuessData);
                return strlen(outBuffer);
            }
        }
        //        else if (!parseState) {
        //            OledClear(OLED_COLOR_BLACK);
        //            OledDrawString(ERROR_STRING_PARSING);
        //            OledUpdate();
        //            aState = INVALID;
        //        }
        break;

    case AGENT_STATE_INVALID:
        break;

    case AGENT_STATE_LOST:
        break;

    case AGENT_STATE_WON:
        break;
    }

    return FALSE;
}

/**
 * StateCheck() returns a 4-bit number indicating the status of that agent's ships. The smallest
 * ship, the 3-length one, is indicated by the 0th bit, the medium-length ship (4 tiles) is the
 * 1st bit, etc. until the 3rd bit is the biggest (6-tile) ship. This function is used within
 * main() to update the LEDs displaying each agents' ship status. This function is similar to
 * Field::FieldGetBoatStates().
 * @return A bitfield indicating the sunk/unsunk status of each ship under this agent's control.
 *
 * @see Field.h:FieldGetBoatStates()
 * @see Field.h:BoatStatus
 */
uint8_t AgentGetStatus(void)
{
    return FieldGetBoatStates(&mField);
}

/**
 * This function returns the same data as `AgentCheckState()`, but for the enemy agent.
 * @return A bitfield indicating the sunk/unsunk status of each ship under the enemy agent's
 *         control.
 *
 * @see Field.h:FieldGetBoatStates()
 * @see Field.h:BoatStatus
 */
uint8_t AgentGetEnemyStatus(void)
{
    return FieldGetBoatStates(&tField);
}




