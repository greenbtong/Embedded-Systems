#include "cmsis_os.h"                            // CMSIS RTOS header file
#include "stm32f4xx.h"
#include "my_headers.h"

void Blink_LED_Thread (void const *argument); // Declares the main thread function that is defined later in the code
osThreadId tid_Blink_LED_Thread; // Declares an ID that we will associate with the thread and which allows easy reference to it when using some of the OS functions.
osThreadDef (Blink_LED_Thread, osPriorityNormal, 1, 0); // Declares the main thread object that we will use later. The parameters can be used to adjust certain properties, such as the priority of a thread and how many instances of it exist.

void Button_Thread (void const *argument); // Declares the main thread function that is defined later in the code
osThreadId tid_Button_Thread; // Declares an ID that we will associate with the thread and which allows easy reference to it when using some of the OS functions.
osThreadDef (Button_Thread, osPriorityNormal, 1, 0); // Declares the main thread object that we will use later. The parameters can be used to adjust certain properties, such as the priority of a thread and how many instances of it exist.

/*-------------------------------------------------------------------------
 *      Blink LED Thread
 *-----------------------------------------------------------------------*/
 
// Code to define the thread function to initialise the main thread - this initialise function is called from the “main.c” file to start the thread.
int Init_Blink_LED_Thread (void) {

  tid_Blink_LED_Thread = osThreadCreate (osThread(Blink_LED_Thread), NULL); // Creates the main thread object that we have declared and assigns it the thread ID that we have declared.
  if(!tid_Blink_LED_Thread) return(-1); // Checks to make sure the thread has been created.
  
  return(0);
}

// Code to define the operation of the main thread. This is effectively the code that was in the infinite FOR loop of our previous blinky program.
void Blink_LED_Thread (void const *argument) {

   
	uint8_t LED_on = 1; // Defines parameter for LED on
	uint8_t LED_off = 0; // Defines parameter for LED off
	
  while (1) { // Creates an infinite loop so that the blinking never terminates
		
		Blink_LED(LED_on); // Blinks the green LED on once
		
		osDelay(1000); // Uses the built in delay function for the OS to create a 1 second delay. The fundamental delay is specified in the “RTX_conf_CM.c” file and usually defaults to 1ms.
		
		Blink_LED(LED_off); // Blinks the green LED on once
		
		osDelay(1000); // Uses the built in delay function for the OS to create a 1 second delay. The fundamental delay is specified in the “RTX_conf_CM.c” file and usually defaults to 1ms.	
		
		osThreadYield(); // This function tells the RTOS that when the thread gets to this stage the RTOS should suspend this thread and run the next thread that is ready to run. If there is no other thread ready (which is the case with this simple program since we only have one thread) then the calling thread continues. This function effectively forces the RTOS to reschedule and is useful in more complex systems and scheduling policies.
		
  }

}

/*-------------------------------------------------------------------------
 *      Red LED on when button pressed thread Thread
 *-----------------------------------------------------------------------*/
 
// Code to define the thread function to initialise the main thread - this initialise function is called from the “main.c” file to start the thread.
int Init_Button_Thread (void) {

  tid_Button_Thread = osThreadCreate (osThread(Button_Thread), NULL); // Creates the main thread object that we have declared and assigns it the thread ID that we have declared.
  if(!tid_Button_Thread) return(-1); // Checks to make sure the thread has been created.
  
  return(0);
}

// Code to define the operation of the main thread. This is effectively the code that was in the infinite FOR loop of our previous blinky program.
void Button_Thread (void const *argument) {

	uint8_t LED_on = 1; // Defines parameter for LED on
	uint8_t LED_off = 0; // Defines parameter for LED off
   	
  while (1) { // Creates an infinite loop so that the blinking never terminates
		
		// Checks the state of the push-button and only turns the red LED on if the button has only just been pressed, which is indicated by the state of the red LED. 
		if(((GPIOA->IDR & 0x00000001) == 0x00000001) & ((GPIOD->ODR & (1<<14)) != (1<<14))){
					
			Red_LED(LED_on); // Turn red LED on
		
		}
		// Checks the state of the push-button and only turns the red LED off if the button has only just been released, which is indicated by the state of the red LED. 		
		else if(((GPIOA->IDR & 0x00000001) != 0x00000001) & ((GPIOD->ODR & (1<<14)) == (1<<14))){
		
			Red_LED(LED_off); // Turn red LED off
			
		}		
		osThreadYield(); // This function tells the RTOS that when the thread gets to this stage the RTOS should suspend this thread and run the next thread that is ready to run. If there is no other thread ready (which is the case with this simple program since we only have one thread) then the calling thread continues. This function effectively forces the RTOS to reschedule and is useful in more complex systems and scheduling policies.
		
  }

}
