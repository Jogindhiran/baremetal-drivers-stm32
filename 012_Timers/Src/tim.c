//Hardware Timer Driver
#include "stm32f4xx.h"

#define TIM2EN			(1U<<0)
#define CR1_CEN			(1U<<0) //Counter is disabled (Timer is enabled)
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

