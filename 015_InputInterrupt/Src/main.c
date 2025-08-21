//Variation: Program to receive a number key in UART and blink on board LED as many times as the key
//EXTI Interrupt: Transmitting a message to UART when the blue button is pressed

#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "uart.h"
#include "adc.h"
#include "systick.h"
#include "tim.h"
#include "exti.h"


#define GPIOAEN			(1U<<0)
#define PIN5			(1U<<5)

#define LED_PIN			PIN5

char key;
int key_int;

void keyRead(int keyint);
static void exti_callback(void);

int main(void) {
	//1.Enable clock access to GPIOA
	RCC->AHB1ENR |= GPIOAEN;

	//2.Set PA5 as output pin
	GPIOA->MODER |= (1U<<10);
	GPIOA->MODER &= ~(1U<<11);

	uart2_rxtx_init();

	pc13_exti_init();

	while(1) {
		key = uart2_read();
		key_int = key - '0';
		keyRead(key_int);
	}
}

static void exti_callback(void) {
	printf("BTN Pressed...\n\r");

	//GPIOA->ODR ^= LED_PIN;	//LED toggle

}

void EXTI15_10_IRQHandler(void) {
	if((EXTI->PR & LINE13) != 0) { //PR is pending register
		/*Clear PR flag*/
		EXTI->PR |= LINE13;

		//Write the required code
		exti_callback();
	}
}

void keyRead(int keyint) {
	int i,j;
	for(i=keyint; i>0; i--) {
		GPIOA->ODR |= LED_PIN;
		for(j=0;j<500000;j++);
		GPIOA->ODR &= ~LED_PIN;
		for(j=0;j<500000;j++);
	}
}
