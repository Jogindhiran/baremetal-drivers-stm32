#include "exti.h"

#define GPIOCEN		(1U<<2)
#define SYSCFGEN	(1U<<14)

void pc13_exti_init(void) {
	/*1. Disable global interrupts*/
	__disable_irq(); //Inbuilt function in Cortex-M.h series MCU

	/*2. Enable clock access for GPIOC*/
	RCC->AHB1ENR |= GPIOCEN;

	/*Set PC13 as input pin*/
	GPIOC->MODER &= ~(1U<<26);
	GPIOC->MODER &= ~(1U<<27);

	/*3. Enable clock access to SYSCFG (System Configuration Register contains EXTI)*/
	RCC->APB2ENR |= SYSCFGEN;

	/*4. Select PORTC for EXTI13*/
	SYSCFG->EXTICR[3] |= (1U<<5);

	/*5. Unmask EXTI13*/
	EXTI->IMR |= (1U<<13); //Enabling interrupt access to EXTI13

	/*6. Select falling edge trigger*/
	EXTI->FTSR |= (1U<<13);

	/*7. Enable EXTI13 line in NVIC*/
	NVIC_EnableIRQ(EXTI15_10_IRQn);

	/*8. Enable global interrupts*/
	__enable_irq();
}
