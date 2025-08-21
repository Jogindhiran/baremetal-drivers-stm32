#include "stm32f4xx.h"


#define SYSTICK_LOAD_VAL			16000	//Cycles per ms
#define CTRL_ENABLE					(1U<<0) //Control Register of SysTick
#define CTRL_CLKSRC					(1U<<2)
#define CTRL_COUNTFLAG				(1U<<16)

void systickDelayMS(int delay) {
	/*1. Reload with number of clocks per millisecond*/
	SysTick->LOAD = SYSTICK_LOAD_VAL;

	/*2. Clear SysTick Current Value Register*/
	SysTick->VAL = 0;

	/*3. Enable SysTick and select internal clock source*/
	SysTick->CTRL = CTRL_ENABLE | CTRL_CLKSRC;

	for(int i=0;i<delay;i++) {
		/*Wait until the COUNTFLAG is set (1)*/
		while((SysTick->CTRL & CTRL_COUNTFLAG) == 0);
	}

	/*4. Disable SysTick Timer*/
	SysTick->CTRL = 0;
}

