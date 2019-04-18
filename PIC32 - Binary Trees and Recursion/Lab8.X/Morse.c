/*
 * File:   Morse.c
 * Author: Brian
 *
 * Created on May 30, 2017, 10:10 PM
 */

#include "Morse.h"
#include "Tree.h"
#include "Buttons.h"
#include "Oled.h"

#include <stdio.h>

const char tree[63] = {'\0', 'E', 'I', 'S', 'H', '5', '4', 'V', '\0', '3', 'U', 'F', '\0', '\0', '\0', '\0', '2',
    'A', 'R', 'L', '\0', '\0', '\0', '\0', '\0', 'W', 'P', '\0', '\0', 'J', '\0', '1',
    'T', 'N', 'D', 'B', '6', '\0', 'X', '\0', '\0', 'K', 'C', '\0', '\0', 'Y', '\0', '\0',
    'M', 'G', 'Z', '7', '\0', 'Q', '\0', '\0', 'O', '\0', '8', '\0', '\0', '9', '0'};

static Node *treeVal;
static Node *currentVal;
static int count = 0;

enum {
    WAITING,
    DOT,
    DASH,
    INTER_LETTER,
} mState = WAITING;

/**
 * This function initializes the Morse code decoder. This is primarily the generation of the
 * Morse tree: a binary tree consisting of all of the ASCII alphanumeric characters arranged
 * according to the DOTs and DASHes that represent each character. Traversal of the tree is done by
 * taking the left-child if it is a dot and the right-child if it is a dash. If the morse tree is
 * successfully generated, SUCCESS is returned, otherwise STANDARD_ERROR is returned. This function
 * also initializes the Buttons library so that MorseCheckEvents() can work properly.
 * @return Either SUCCESS if the decoding tree was successfully created or STANDARD_ERROR if not.
 */
int MorseInit(void)
{
    ButtonsInit();
    treeVal = TreeCreate(6, tree);
    currentVal = treeVal;
    if (treeVal == NULL) {
        return STANDARD_ERROR;
    } else {
        return SUCCESS;
    }
}

/**
 * MorseDecode decodes a Morse string by iteratively being passed MORSE_CHAR_DOT or MORSE_CHAR_DASH.
 * Each call that passes a DOT or DASH returns a SUCCESS if the string could still compose a
 * Morse-encoded character. Passing in the special MORSE_CHAR_END_OF_CHAR constant will terminate
 * decoding and return the decoded character. During that call to MorseDecode() the return value
 * will be the character that was decoded or STANDARD_ERROR if it couldn't be decoded. Another
 * special value exists, MORSE_CHAR_DECODE_RESET, which will clear the stored state. When a
 * MORSE_CHAR_DECODE_RESET is done, SUCCESS will be returned. If the input is not a valid MorseChar
 * then the internal state should be reset and STANDARD_ERROR should be returned.
 * 
 * @param in A value from the MorseChar enum which specifies how to traverse the Morse tree.
 * 
 * @return Either SUCCESS on DECODE_RESET or when the next traversal location is still a valid
 *         character, the decoded character on END_OF_CHAR, or STANDARD_ERROR if the Morse tree
 *         hasn't been initialized, the next traversal location doesn't exist/represent a character,
 *         or `in` isn't a valid member of the MorseChar enum.
 */
char MorseDecode(MorseChar in)
{
    char forNow;
    // Check if dot
    if (in == MORSE_CHAR_DOT) {

        // Check if left is not NULL
        if (currentVal->leftChild == NULL) {
            return STANDARD_ERROR;
        } else {
            currentVal = currentVal->leftChild;
            return SUCCESS;
        }

    } else if (in == MORSE_CHAR_DASH) {

        // Check if dash. Check if right is not NULL
        if (currentVal->rightChild == NULL) {
            return STANDARD_ERROR;
        } else {
            currentVal = currentVal->rightChild;
            return SUCCESS;
        }

    } else if (in == MORSE_CHAR_END_OF_CHAR) {

        // return char and restart tree
        forNow = currentVal->data;
        currentVal = treeVal;
        return forNow;

    } else if (in == MORSE_CHAR_DECODE_RESET) {

        // restart tree
        currentVal = treeVal;
        return SUCCESS;

    } else {
        return STANDARD_ERROR;
    }

}

/**
 * This function calls ButtonsCheckEvents() once per call and returns which, if any,
 * of the Morse code events listed in the enum above have been encountered. It checks for BTN4
 * events in its input and should be called at 100Hz so that the timing works. The
 * length that BTN4 needs to be held down for a dot is >= 0.25s and < 0.50s with a dash being a button
 * down event for >= 0.5s. The button uptime various between dots/dashes (>= .5s), letters
 * (>= 1s), and words (>= 2s).
 *
 * @note This function assumes that the buttons are all unpressed at startup, so that the first
 *       event it will see is a BUTTON_EVENT_*DOWN.
 *
 * So pressing the button for 0.1s, releasing it for 0.1s, pressing it for 0.3s, and then waiting
 * will decode the string '.-' (A). It will trigger the following order of events:
 * 9 MORSE_EVENT_NONEs, 1 MORSE_EVENT_DOT, 39 MORSE_EVENT_NONEs, a MORSE_EVENT_DASH, 69
 * MORSE_EVENT_NONEs, a MORSE_EVENT_END_CHAR, and then MORSE_EVENT_INTER_WORDs.
 * 
 * @return The MorseEvent that occurred.
 */
MorseEvent MorseCheckEvents(void)
{
    // Timer
    count++;

    // State machine
    switch (mState) {

    case WAITING:
        count = 0;

        // If button 4 is down move to dot
        if (ButtonsCheckEvents() == BUTTON_EVENT_4DOWN) {
            count = 0;
            mState = DOT;
        }
        break;

    case DOT:

        // If button 4 is up move to inter letter and return dot
        if (ButtonsCheckEvents() == BUTTON_EVENT_4UP) {
            count = 0;
            mState = INTER_LETTER;
            return MORSE_EVENT_DOT;
        } else if (count >= MORSE_EVENT_LENGTH_DOWN_DOT) {
            // Check if dash (move to dash)
            mState = DASH;
        }

        break;

    case DASH:

        // If button 4 is up move to inter letter and return dash
        if (ButtonsCheckEvents() == BUTTON_EVENT_4UP) {
            count = 0;
            mState = INTER_LETTER;
            return MORSE_EVENT_DASH;
        }

        break;

    case INTER_LETTER:

        // If count goes base event timer goes back to waiting and returns inter word
        if (count >= MORSE_EVENT_LENGTH_UP_INTER_LETTER_TIMEOUT) {
            mState = WAITING;
            return MORSE_EVENT_INTER_WORD;
        } else if (ButtonsCheckEvents() == BUTTON_EVENT_4DOWN) {
            // If button 4 is down check if count is over inter letter
            if (count >= MORSE_EVENT_LENGTH_UP_INTER_LETTER) {
                count = 0;
                mState = DOT;
                return MORSE_EVENT_INTER_LETTER;
            } else {
                count = 0;
                mState = DOT;
            }
        }

        break;
    }
    return MORSE_EVENT_NONE;
}
