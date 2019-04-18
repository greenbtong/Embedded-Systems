/*-------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function - Blinky
 *-----------------------------------------------------------------------*/

#define osObjectsPublic                   // Define objects in main module
#include "osObjects.h"                    // RTOS object definitions
#include "stm32f4xx.h"
#include "my_headers.h"
#include "Thread.h"

int main (void) {
 
 osKernelInitialize ();	//Initialize CMSIS-RTOS
	
  	
	/* Initialise any peripherals or system components */

// Initialize the LED and button	
	Initialise_LED_and_button();

	/* Initialise any threads */

// Initialise the main thread to blink the LED’s
	Init_Blink_LED_Thread();
	Init_Button_Thread();
	
	osKernelStart ();	// start thread execution 
	
	while(1){}; // While loop so the program doesn’t terminate
}
