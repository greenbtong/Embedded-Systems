//// task 4

//#include "stm32f4xx.h"
//int main(void){

//	uint32_t ii;

//	// Initialize GPIO Port for LEDs
//	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // Enable GPIOD clock
//	GPIOD->MODER |= GPIO_MODER_MODER12_0; // GPIOD pin 12 output

//	for(;;){
//		GPIOD->BSRR = 1<<12; // Turn on the green LED
//	
//		for(ii=0;ii<26000000;ii++){} //FOR loop to implement a 1 second  delay with a clock speed of 168MHz
//	
//		GPIOD->BSRR = 1<<(12+16); // Turn off the green LED
//	
//		for(ii=0;ii<26000000;ii++){} //FOR loop to implement a 1 second delay with a clock speed of 168MHz
//	}
//}

// task 6

#include "stm32f4xx.h"

int main(void){
	
	uint32_t ii;
	
	//Initialize GPIO port for LEDs
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // Enable GPIOD clock 
	GPIOD->MODER |= GPIO_MODER_MODER13_0;  // GPIOD pin 13 output
	GPIOD->MODER |= GPIO_MODER_MODER15_0;  // GPIOD pin 15 output
	
	for(;;) {
		
		GPIOD->BSRR = 1<<(15+16); // Turn off the blue LED
		GPIOD->BSRR = 1<<13; // Turn on the orange LED
		
		for(ii=0;ii<26000000*2;ii++){} //FOR loop to implement a 1 second delay with a clock speed of 168MHz

		GPIOD->BSRR = 1<<(13+16); // Turn off the orange LED
		GPIOD->BSRR = 1<<15; // Turn on the blue LED

		for(ii=0;ii<26000000*2;ii++){} //FOR loop to implement a 1 second delay with a clock speed of 168MHz
	}
}