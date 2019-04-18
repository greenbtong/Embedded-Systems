// **** Include libraries here ****
// Standard libraries

//CMPE13 Support Library
#include "BOARD.h"

// Microchip libraries
#include <xc.h>
#include <plib.h>

// User libraries
#include "Stack.h"
#include "string.h"

// **** Set macros and preprocessor directives ****

// **** Define global, module-level, or external variables here ****
#define INPUT_LIMIT 60
// **** Declare function prototypes ****
/*
 * Function prototype for ProcessBackspaces() - This function should be defined
 * below main after the related comment.
 */
int ProcessBackspaces(char *rpn_sentence);

// If this code is used for testing, rename main to something we can call from our testing code. We
// also skip all processor configuration and initialization code.
#ifndef LAB4_TESTING

int main()
{
    BOARD_Init();
#else

int their_main(void)
{
#endif // LAB4_TESTING

    /******************************** Your custom code goes below here *******************************/
    // Greeting
    printf("Welcome to Brian's RPN calculator.\n");
    printf("Enter floats and + - / * in RPN format: \n");

    // Variables
    struct Stack stack;
    char input[INPUT_LIMIT + 1];
    char *token;
    float temp, value;
    int i, error;

    // Input, RPN calculation, and output
    while (1) {
        error = 0; // 0 if there are no errors, 1 if there are no errors
        StackInit(&stack); // Initialize

        // Clear input array
        for (i = 0; i < INPUT_LIMIT + 1; i++) {
            input[i] = '\0';
        }

        fgets(input, INPUT_LIMIT + 1, stdin); // Reads in user input until newline
        ProcessBackspaces(input); // Backspace function 
        token = strtok(input, " "); // Split input into tokens

        // Do while loop until input is all split up into tokens
        do {
            temp = atof(token); // Convert string to double

            // Check if the Stack isn't full
            if (StackIsFull(&stack)) {
                printf("ERROR: No more room on stack.\n");
                error = 1;
                break;
            }

            // Check if the input is valid. If it is, it will be pushed 
            if (token[0] == '0' || temp != 0) {
                StackPush(&stack, temp);
            } else {

                /* 
                 * If it isn't math expression, ERROR 
                 * Else if there is not enough values to complete an expression, ERROR
                 */
                if (token[0] != '+' && token[0] != '-' && token[0] != '*' && token[0] != '/') {
                    printf("ERROR: Invalid character in RPN string. \n");
                    error = 1;
                    break;
                } else if (StackGetSize(&stack) < 2) {
                    printf("ERROR: Not enough operands before operator.\n");
                    error = 1;
                    break;
                }

                // pop the top two values from the stack
                StackPop(&stack, &temp); // Top of stack
                StackPop(&stack, &value); // Second from top of stack

                // Complete expression
                if (token[0] == '+') {
                    value += temp;
                } else if (token[0] == '-') {
                    value -= temp;
                } else if (token[0] == '*') {
                    value *= temp;
                } else if (token[0] == '/') {
                    value /= temp;
                }
                StackPush(&stack, value); // Push answer back into stack
            }
            token = strtok(NULL, " "); // Get next string from the input
        } while (token);

        /*
         * If there was an error somewhere earlier, CONTINUE
         * Else if the stack has more than one value, ERROR
         * Else print out the answer (means it worked)
         */
        if (error == 1) {
            continue;
        } else if (StackGetSize(&stack) > 1) {
            printf("ERROR: Invalid RPN calculation: more or less than one item in the stack.\n");
        } else {
            StackPop(&stack, &temp);
            printf("Answer to your expression: %.3f\n", (double) temp);
        }
    }

    /*************************************************************************************************/

    // You can never return from main() in an embedded system (one that lacks an operating system).
    // This will result in the processor restarting, which is almost certainly not what you want!
    while (1);
}

/**
 * Extra Credit: Define ProcessBackspaces() here - This function should read through an array of
 * characters and when a backspace character is read it should replace the preceding character with
 * the following character. It should be able to handle multiple repeated backspaces and also
 * strings with more backspaces than characters. It should be able to handle strings that are at
 * least 256 characters in length.
 * @param rpn_sentence The string that will be processed for backspaces. This string is modified in
 *                     place.
 * @return Returns the size of the resultant string in `rpn_sentence`.
 */
int ProcessBackspaces(char *rpn_sentence)
{
    // Variables 
    int i, count; 
    char *temp;
    
    /*
     * Runs through the string and finds the '\b' 
     * Rather than backspacing replace what would of been with '\b' 
     */
    for (i = 0; i < strlen(rpn_sentence); i++) {
        if (rpn_sentence[i] == '\b') {
            count = 1;
            if (i - count < 0) {
                continue;
            } else {
                while (count != 0) {
                    if (rpn_sentence[i - count] == '\b') {
                        count++;
                    } else {
                        rpn_sentence[i - count] = '\b';
                        count = 0;
                    }
                    if (i - count < 0) {
                        count = 0;
                    }
                }

            }
        }
    }
    
    // Token the string by '\b'
    strtok(rpn_sentence, "\b");
    temp = strtok(NULL, "\b");
    
    // Add the string together 
    while (temp) {
        strcat(rpn_sentence, temp);
        temp = strtok(NULL, "\b");
    }
    return 0;
}
