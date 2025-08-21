#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "uart.h"

//Program to read the key and turn on LED if key '1' is pressed
//Variant: Read the key and blink the LED that many times - 19/07/2025

#define GPIOAEN			(1U<<0)
#define GPIOA_5			(1U<<5)

#define LED_PIN			GPIOA_5
char key;
int key_int;

void keyRead(int keyint);

int main(void) {

	//1. Enable clock access to GPIOA
	RCC->AHB1ENR |= GPIOAEN;

	//2. Set PA5 as output pin
	GPIOA->MODER |= (1U<<10);
	GPIOA->MODER &= ~(1U<<11);

	uart2_rxtx_init();

	while(1) {
		key = uart2_read();
		key_int = key - '0';
		keyRead(key_int);
//		if(key == '1') {
//			GPIOA->ODR |= LED_PIN; //Turn on LED
//		}
//		else {
//			GPIOA->ODR &= ~LED_PIN; //Turn off LED
//		}

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

