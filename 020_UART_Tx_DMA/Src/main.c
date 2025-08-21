#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "uart.h"

//Program to read the key and turn on LED if key '1' is pressed

#define GPIOAEN			(1U<<0)
#define GPIOA_5			(1U<<5)

#define LED_PIN			GPIOA_5
char key;

static void dma_callback(void);

int main(void) {
	char message[31] = "Hello from STM32 DMA Transfer\n\r";

	//1. Enable clock access to GPIOA
	RCC->AHB1ENR |= GPIOAEN;

	//2. Set PA5 as output pin
	GPIOA->MODER |= (1U<<10);
	GPIOA->MODER &= ~(1U<<11);

	uart2_tx_init();
	dma1_stream6_init((uint32_t) message, (uint32_t) &USART2->DR, (uint32_t) 31);

	while(1) {

	}
}

static void dma_callback(void) {

	GPIOA->ODR |= LED_PIN; //Turn on LED once the transfer is complete

}

void DMA1_Stream6_IRQHandler(void) {
	/*Check for transfer complete interrupt*/
	if(DMA1->HISR & HISR_TCIF6) {
		/*Clear flag*/
		DMA1->HIFCR |= HIFCR_CTCIF6;

		//Do something
		dma_callback();
	}
}




