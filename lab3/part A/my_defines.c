#include "stm32f4xx.h"

// Definition for the function to initialise the LED
void Initialise_LED(void){

	// Initialize GPIO Port for LEDs
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // Enable Port D clock 
	GPIOD->MODER |= GPIO_MODER_MODER12_0; // Port D.12 output - green LED

}

// Definition for the function to blink the LED
void Blink_LED(uint8_t LED_state){
	uint32_t ii;
	if(LED_state == 1){ // Checks to see if the request is to turn the LED on or off
		GPIOD->BSRR = 1<<12; // Turn on the green LED
	}
	else{
		GPIOD->BSRR = 1<<(12+16); // Turn off the green LED
	}
	for(ii=0;ii<26000000;ii++){} // FOR loop to implement a 1 second delay with a clock speed of 168MHz
}

// delay 
void Delay_in_seconds(uint8_t time_in_seconds){
	
		uint32_t ii;
	
		for(ii=0;ii<(time_in_seconds*26000000);ii++){} // FOR loop to implement a 1 second delay with a clock speed of 168MHz

		}
