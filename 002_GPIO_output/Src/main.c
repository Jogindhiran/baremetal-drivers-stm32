#include "stm32f4xx.h"

//GPIOAEN, PIN5 and LED_PIN are variables defined by us. It is not there in the header file

#define GPIOAEN			(1U<<0) //bit 0 = 1 which means IO port A clock is enabled (Reference Manual)
#define PIN5			(1U<<5)
#define LED_PIN			PIN5

int main(void) {
	RCC->AHB1ENR |= GPIOAEN; //clock enabled for port A

	//Make PA5 as output pin (01 - 11th bit = 0 and 10th bit = 1) (2y,2y+1) bit y = 5 here
	GPIOA->MODER |= (1U<<10); 	// 0000 | 00(1)0 = 0010
	GPIOA->MODER &= ~(1U<<11); 	// 0010 & 1(0)11 = 0(01)0

	while(1) {
		GPIOA->ODR ^= LED_PIN; //Toggling LED pin
		for(int i=0;i<900000;i++); //A pseudo delay
	}
}
