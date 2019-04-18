/*
 * File:   Protocol.c
 * Author: Brian Tong and Sam Hanawalt
 *
 * Created on June 5, 2017, 12:34 AM
 */

// Standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// User libraries
#include "Protocol.h"

// **** Define any module-level, global, or external variables here ****
#define TRUE 1
#define FALSE 0

typedef enum {
    WAITING,
    RECORDING,
    FIRST_CHECKSUM_HALF,
    SECOND_CHECKSUM_HALF,
    NEWLINE
} myState;

typedef struct protoData {
    int INDEX;
    char SENT[PROTOCOL_MAX_MESSAGE_LEN];
    myState mState;
    uint8_t HASH;
} protoData;

static protoData stateData = {0,
    {}, WAITING, 0};
static char tempM[PROTOCOL_MAX_PAYLOAD_LEN];
static char *temp;
static char *token[4];
static uint8_t tempSs = 0;
static int p;


// **** Declare any function prototypes here ****
static uint8_t stringToHash(char *data);
static uint8_t toUint(char in);

/**
 * Encodes the coordinate data for a guess into the string `message`. This string must be big
 * enough to contain all of the necessary data. The format is specified in PAYLOAD_TEMPLATE_COO,
 * which is then wrapped within the message as defined by MESSAGE_TEMPLATE. The final length of this
 * message is then returned. There is no failure mode for this function as there is no checking
 * for NULL pointers.
 * @param message The character array used for storing the output. Must be long enough to store the
 *                entire string, see PROTOCOL_MAX_MESSAGE_LEN.
 * @param data The data struct that holds the data to be encoded into `message`.
 * @return The length of the string stored into `message`.
 */
int ProtocolEncodeCooMessage(char *message, const GuessData *data)
{
    sprintf(tempM, PAYLOAD_TEMPLATE_COO, data->row, data->col);
    tempSs = stringToHash(tempM);
    sprintf(message, MESSAGE_TEMPLATE, tempM, tempSs);
    return strlen(message);
}

/**
 * Follows from ProtocolEncodeCooMessage above.
 */
int ProtocolEncodeHitMessage(char *message, const GuessData *data)
{
    sprintf(tempM, PAYLOAD_TEMPLATE_HIT, data->row, data->col, data->hit);
    tempSs = stringToHash(tempM);
    sprintf(message, MESSAGE_TEMPLATE, tempM, tempSs);
    return strlen(message);
}

/**
 * Follows from ProtocolEncodeCooMessage above.
 */
int ProtocolEncodeChaMessage(char *message, const NegotiationData *data)
{
    sprintf(tempM, PAYLOAD_TEMPLATE_CHA, data->encryptedGuess, data->hash);
    tempSs = stringToHash(tempM);
    sprintf(message, MESSAGE_TEMPLATE, tempM, tempSs);
    return strlen(message);
}

/**
 * Follows from ProtocolEncodeCooMessage above.
 */
int ProtocolEncodeDetMessage(char *message, const NegotiationData *data)
{
    sprintf(tempM, PAYLOAD_TEMPLATE_DET, data->guess, data->encryptedGuess);
    tempSs = stringToHash(tempM);
    sprintf(message, MESSAGE_TEMPLATE, tempM, tempSs);
    return strlen(message);
}

/**
 * This function decodes a message into either the NegotiationData or GuessData structs depending
 * on what the type of message is. This function receives the message one byte at a time, where the
 * messages are in the format defined by MESSAGE_TEMPLATE, with payloads of the format defined by
 * the PAYLOAD_TEMPLATE_* macros. It returns the type of message that was decoded and also places
 * the decoded data into either the `nData` or `gData` structs depending on what the message held.
 * The onus is on the calling function to make sure the appropriate structs are available (blame the
 * lack of function overloading in C for this ugliness).
 *
 * PROTOCOL_PARSING_FAILURE is returned if there was an error of any kind (though this excludes
 * checking for NULL pointers), while
 * 
 * @param in The next character in the NMEA0183 message to be decoded.
 * @param nData A struct used for storing data if a message is decoded that stores NegotiationData.
 * @param gData A struct used for storing data if a message is decoded that stores GuessData.
 * @return A value from the UnpackageDataEnum enum.
 */
ProtocolParserStatus ProtocolDecode(char in, NegotiationData *nData, GuessData *gData)
{
    // switch
    switch (stateData.mState) {

        // start read $
    case WAITING:

        // check if $
        if (in == '$') {
            stateData.mState = RECORDING;
            stateData.INDEX = 0;
            return PROTOCOL_PARSING_GOOD;
        } else {
            stateData.mState = WAITING;
            return PROTOCOL_WAITING;
        }
        break;

        // record char after $ to *
    case RECORDING:

        // check if *
        if (in == '*') {
            stateData.mState = FIRST_CHECKSUM_HALF;
            return PROTOCOL_PARSING_GOOD;
        } else {

            stateData.SENT[stateData.INDEX] = in;
            stateData.INDEX += 1;
            stateData.mState = RECORDING;
            return PROTOCOL_PARSING_GOOD;
        }
        break;

        // check hash
    case FIRST_CHECKSUM_HALF:

        // check if valid input
        stateData.HASH |= toUint(in);
        if (stateData.HASH != FALSE) {

            // top 4 bit
            stateData.HASH = stateData.HASH << 4;
            stateData.mState = SECOND_CHECKSUM_HALF;
            return PROTOCOL_PARSING_GOOD;
        } else {
            stateData.mState = WAITING;
            return PROTOCOL_PARSING_FAILURE;
        }
        break;

        // check hash to sum
    case SECOND_CHECKSUM_HALF:

        // bot 4 bit
        stateData.HASH |= (toUint(in) & 0x0F);

        // check sum
        if ((toUint(in) != FALSE) && (stateData.HASH == stringToHash(stateData.SENT))) {
            stateData.SENT[stateData.INDEX] = '\0';
            stateData.mState = NEWLINE;
            return PROTOCOL_PARSING_GOOD;
        } else {
            stateData.mState = WAITING;
            return PROTOCOL_PARSING_FAILURE;
        }
        break;

        // determine what is passed
    case NEWLINE:

        // split string into tokens
        p = 0;
        temp = strtok(stateData.SENT, ",");
        for (p = 0; temp != NULL; p++) {
            token[p] = temp;
            temp = strtok(NULL, ",");
        }

        // determine if pass or fail
        if ((in == '\n') && ((strcmp(token[0], "DET") == 0) || (strcmp(token[0], "CHA") == 0)
                || (strcmp(token[0], "COO") == 0) || (strcmp(token[0], "HIT") == 0))) {

            if (strcmp(token[0], "DET") == 0) {
                nData->guess = atoi(token[1]);
                nData->encryptionKey = atoi(token[2]);
                return PROTOCOL_PARSED_DET_MESSAGE;
            } else if (strcmp(token[0], "CHA") == 0) {
                nData->encryptedGuess = atoi(token[1]);
                nData->hash = atoi(token[2]);
                return PROTOCOL_PARSED_CHA_MESSAGE;
            } else if (strcmp(token[0], "COO") == 0) {
                gData->row = atoi(token[1]);
                gData->col = atoi(token[2]);
                return PROTOCOL_PARSED_COO_MESSAGE;
            } else if (strcmp(token[0], "HIT") == 0) {
                gData->row = atoi(token[1]);
                gData->col = atoi(token[2]);
                gData->hit = atoi(token[3]);
                return PROTOCOL_PARSED_HIT_MESSAGE;
            }
            stateData.mState = WAITING;
        } else {
            stateData.mState = WAITING;
            return PROTOCOL_PARSING_FAILURE;
        }
        break;
    }
    return 0;
}

/**
 * This function generates all of the data necessary for the negotiation process used to determine
 * the player that goes first. It relies on the pseudo-random functionality built into the standard
 * library. The output is stored in the passed NegotiationData struct. The negotiation data is
 * generated by creating two random 16-bit numbers, one for the actual guess and another for an
 * encryptionKey used for encrypting the data. The 'encryptedGuess' is generated with an
 * XOR(guess, encryptionKey). The hash is simply an 8-bit value that is the XOR() of all of the
 * bytes making up both the guess and the encryptionKey. There is no checking for NULL pointers
 * within this function.
 * @param data The struct used for both input and output of negotiation data.
 */
void ProtocolGenerateNegotiationData(NegotiationData *data)
{
    // produce random encryption/guess 
    data->guess = rand() & 0xFFFF;
    data->encryptionKey = rand() & 0xFFFF;
    data->encryptedGuess = data->guess ^ data->encryptionKey;
    data->hash = (((data->guess >> 8) ^ (data->guess & 0xFF))
            ^ ((data->encryptionKey >> 8) ^ (data->encryptionKey & 0xFF)));
}

/**
 * Validates that the negotiation data within 'data' is correct according to the algorithm given in
 * GenerateNegotitateData(). Used for verifying another agent's supplied negotiation data. There is
 * no checking for NULL pointers within this function. Returns TRUE if the NegotiationData struct
 * is valid or FALSE on failure.
 * @param data A filled NegotiationData struct that will be validated.
 * @return TRUE if the NegotiationData struct is consistent and FALSE otherwise.
 */
uint8_t ProtocolValidateNegotiationData(const NegotiationData *data)
{
    // check everything (encrypted Guess)
    if (data->encryptedGuess != (data->encryptionKey ^ data->guess)) {
        return FALSE;
    }

    // check if random
    if (data->guess == data->encryptionKey) {
        return FALSE;
    }

    // check data hash
    uint8_t hashTemp = (((data->guess >> 8) ^ (data->guess & 0xFF))
            ^ ((data->encryptionKey >> 8) ^ (data->encryptionKey & 0xFF)));
    if (data->hash != hashTemp) {
        return FALSE;
    } else {
        return TRUE;
    }
}

/**
 * This function returns a TurnOrder enum type representing which agent has won precedence for going
 * first. The value returned relates to the agent whose data is in the 'myData' variable. The turn
 * ordering algorithm relies on the XOR() of the 'encryptionKey' used by both agents. The least-
 * significant bit of XOR(myData.encryptionKey, oppData.encryptionKey) is checked so that if it's a
 * 1 the player with the largest 'guess' goes first otherwise if it's a 0, the agent with the
 * smallest 'guess' goes first. The return value of TURN_ORDER_START indicates that 'myData' won,
 * TURN_ORDER_DEFER indicates that 'oppData' won, otherwise a tie is indicated with TURN_ORDER_TIE.
 * There is no checking for NULL pointers within this function.
 * @param myData The negotiation data representing the current agent.
 * @param oppData The negotiation data representing the opposing agent.
 * @return A value from the TurnOrdering enum representing which agent should go first.
 */
TurnOrder ProtocolGetTurnOrder(const NegotiationData *myData, const NegotiationData *oppData)
{
    uint16_t turnOrder = myData->encryptionKey ^ oppData->encryptionKey;

    // if a tie/player/opponent 
    if (myData->guess == oppData->guess) {
        return TURN_ORDER_TIE;
    } else if (turnOrder & 0x01) {
        if (myData->guess > oppData->guess) {
            return TURN_ORDER_START;
        } else {
            return TURN_ORDER_DEFER;
        }
    } else {
        if (myData->guess < oppData->guess) {
            return TURN_ORDER_START;
        } else {
            return TURN_ORDER_DEFER;
        }
    }
}

/*
 * Extra functions
 */
static uint8_t stringToHash(char *data)
{

    // XOR
    uint8_t sumVal = 0;
    int i;
    for (i = 0; i < strlen(data); i++) {
        sumVal ^= data[i];
    }
    return sumVal;
}

static uint8_t toUint(char in)
{

    // covert valid char to uint8_t value
    if ((in >= '0') && (in <= '9')) {
        return in - 48;
    } else if ((in >= 'A') && (in <= 'F')) {
        return in - 55;
    } else if ((in >= 'a') && (in <= 'f')) {
        return in - 87;
    }
    return FALSE;
}