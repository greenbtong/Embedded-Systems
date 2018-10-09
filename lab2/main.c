// excerise 1

#include "stm32f4xx.h"

int main(void){
	// Initialize GPIO Port for LEDs
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // Enable GPIOD clock
	GPIOD->MODER |= GPIO_MODER_MODER12_0; // GPIOD pin 12 output - green LED

	//Initialize Timer 2
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // Enable timer 2 clock

	TIM2->CR1 &= ~0x00000016; /*Sets the counter as an upcounter*/
	TIM2->CR1 &= ~0x00000008; /*Turn off repeat in CR1 - i.e. use one pulse mode*/
	
	TIM2->PSC = 8400-1; /*Prescaler value - the prescaler clock defaults to twice the APB1 which is running at 42MHz - so the timer clock is 84MHz*/
	/* PSC is frequency 8400/2 = 42 MHZ - 1 this will output to pulse */
	
	TIM2->ARR = 5000-1; /*sets the value in the autoreload register*/
	/* ARR record for 1 sec = 10000, .5 sec = 5000 */
	TIM2->EGR = 1; /*Re-initialises the timer*/

	TIM2->CR1 |= 1; //Enables the counter using the register CR1
	// turn green LED on and off based on timer
	for(;;){
		GPIOD->BSRR = 1<<12; // Turn on the green LED
		
		while((TIM2->SR&0x0001)!=1){}; //TIM2 to implement a .5 second delay
		TIM2->SR &= ~1; //Resets to 0 the update interrupt flag in the register SR

		GPIOD->BSRR = 1<<(12+16); // Turn off the green LED
		
		while((TIM2->SR&0x0001)!=1){}; //TIM2 to implement a .5 second delay
		TIM2->SR &= ~1; //Resets to 0 the update interrupt flag in the register SR 
	}
}

