#include "stm32f4xx.h"

// Program to turn off LED when push button is pressed by the user

#define GPIOAEN			(1U<<0)
#define GPIOCEN			(1U<<2) //For push button

#define PIN5			(1U<<5)
#define PIN13			(1U<<13) //PC13 is Push button in MCU

#define LED_PIN			PIN5
#define BTN_PIN			PIN13 //Push button

int main(void) {
	/*Enable clock access to GPIOA and GPIOC*/
	RCC->AHB1ENR |= GPIOAEN;
	RCC->AHB1ENR |= GPIOCEN;

	/*Set PA5 as output pin (01)*/
	GPIOA->MODER |= (1U<<10);
	GPIOA->MODER &= ~(1U<<11); //10 and 11 for MODER5

	/*Set PC13 as input pin (00)*/
	GPIOA->MODER &= ~(1U<<26);
	GPIOA->MODER &= ~(1U<<27); //26 and 27 for MODER13


	while(1) {

		/*Check if BTN is pressed*/
		if(GPIOC->IDR & BTN_PIN) { //IDR's 13th bit will be 1 when not pressed and BTN_PIN is 1 when not pressed
			GPIOA->BSRR = LED_PIN; //BS5 of BSRR is 1 (PA5 is on)
		}

		else { //BTN_PIN is 0 when pressed
			GPIOA->BSRR = (1U<<21); //BR5 of BSRR is 1 (PA5 is off)
		}
	}
}
