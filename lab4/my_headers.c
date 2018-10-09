#include "stm32f4xx.h"

// Definition for the function to initialise the LED and button
void Initialise_LED_and_button(void){

	// Initialize GPIO Port for LEDs
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // Enable Port D clock 
	GPIOD->MODER |= GPIO_MODER_MODER12_0; // Port D.12 output - green LED
	GPIOD->MODER |= GPIO_MODER_MODER14_0; // Port D.14 output - red LED
	
	//Initialize GPIO for push-button
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // Enable Port A clock
	
}

// Definition for the function to blink the LED
void Blink_LED(uint8_t LED_state){

	if(LED_state == 1){ // Checks to see if the request is to turn the LED on or off

			GPIOD->BSRR = 1<<12; // Turn on the green LED
	
	}
	else{
		
			GPIOD->BSRR = 1<<(12+16); // Turn off the green LED
	
	}
			
}

// Definition for the function to blink the LED
void Red_LED(uint8_t LED_state){

	if(LED_state == 1){ // Checks to see if the request is to turn the LED on or off

			GPIOD->BSRR = 1<<14; // Turn on the red LED
	
	}
	else{
		
			GPIOD->BSRR = 1<<(14+16); // Turn off the red LED
	
	}
			
}
