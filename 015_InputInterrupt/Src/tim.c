//Hardware Timer Driver
#include "stm32f4xx.h"

#define TIM2EN			(1U<<0)
#define TIM3EN			(1U<<1)

#define CR1_CEN			(1U<<0) //Counter is disabled (Timer is enabled)
#define OC_TOGGLE		((1U<<4) | (1U<<5))
#define CCER_CC1E		(1U<<0)

#define GPIOAEN			(1U<<0)
#define AFR5_TIM		(1U<<20)
#define AFR6_TIM		(1U<<25)
#define CCER_CC1S		(1U<<0)

//TIM2 is connected to APB1

//Function for timer 2 to generate timeout after every 1 sec
void tim2_1hz_init(void) {
	/*1.Enable clock access to TIM2*/
	RCC->APB1ENR |= TIM2EN;
	/*2.Set prescaler value*/
	TIM2->PSC = 1600 - 1;		// 16 000 000 / 1 600 = 10 000 (10KHz)
	/*3.Set auto-reload value*/
	TIM2->ARR = 10000 -1;		//10 000/10 000 = 1 (1Hz) (-1 because counting starts from 0 to 9999)
	/*4.Clear counter*/
	TIM2->CNT = 0;
	/*5.Enable timer*/
	TIM2->CR1 = CR1_CEN;
}

void tim2_pa5_output_compare(void) {
	/*Enable clock access to GPIOA*/
	RCC->AHB1ENR |= GPIOAEN;

	/*Set PA5 to alternate function mode*/
	GPIOA->MODER &= ~(1U<<10); //(01)
	GPIOA->MODER |= (1U<<11);

	/*Set PA5 alternate function type to TIM2 CH1 (AF01)*/
	GPIOA->AFR[0] |= AFR5_TIM;

	/*1.Enable clock access to TIM2*/
	RCC->APB1ENR |= TIM2EN;
	/*2.Set prescaler value*/
	TIM2->PSC = 1600 - 1;		// 16 000 000 / 1 600 = 10 000 (10KHz)
	/*3.Set auto-reload value*/
	TIM2->ARR = 10000 -1;		//10 000/10 000 = 1 (1Hz) (-1 because counting starts from 0 to 9999)

	/*Set output compare toggle mode*/
	TIM2->CCMR1 = OC_TOGGLE;

	/*Enable TIM2 CH1 in compare mode*/
	TIM2->CCER |= CCER_CC1E;

	/*4.Clear counter*/
	TIM2->CNT = 0;
	/*5.Enable timer*/
	TIM2->CR1 = CR1_CEN;

}

void tim3_pa6_input_capture(void) {
	/*1.Enable clock access to GPIOA*/
	RCC->AHB1ENR |= GPIOAEN;

	/*2.Set PA6 mode to alternate function mode*/
	GPIOA->MODER &= ~(1U<<12); //(01)
	GPIOA->MODER |= (1U<<13);

	/*3.Set PA6 alternate function type to TIM3_CH1 (AF02)*/
	GPIOA->AFR[0] |= AFR6_TIM;

	/*4.Enable clock access to TIM3*/
	RCC->APB1ENR |= TIM3EN;

	/*5.Set prescaler to 16000*/
	TIM3->PSC = 16000 - 1; //16 000 000 / 16 000 = 1000

	/*6.Set CH1 to input capture mode*/
	TIM3->CCMR1 = CCER_CC1S;

	/*7.Set CH1 to capture at rising edge*/
	TIM3->CCER = CCER_CC1E;

	/*8.Enable TIM3*/
	TIM3->CR1 = CR1_CEN;
}
