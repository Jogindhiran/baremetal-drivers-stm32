#include "stm32f4xx.h"
#include "adc.h"

#define GPIOAEN			(1U<<0)
#define ADC1EN			(1U<<8)
#define ADC_CH1			(1U<<0)	//Channel 1
#define ADC_SEQ_LEN_1	0x00	//Sequence length is 1 (no:of conversions)
#define CR2_ADON		(1U<<0) //Enable ADC by setting ADON pin to 1
#define CR2_SWSTART		(1U<<30)//Start conversion bit to 1
#define SR_EOC			(1U<<1) //Conversion flag = 1 if conversion is complete

void pa1_adc_init(void);
void start_conversion(void);
uint32_t adc_read(void);

//Using PA1 as analog pin
void pa1_adc_init(void) {
	/*****Configure the ADC GPIO pin*****/
	//1. Enable clock access to GPIOA
	RCC->AHB1ENR |= GPIOAEN;

	//2. Set PA1 mode to analog mode (11)
	GPIOA->MODER |= (1U<<2);
	GPIOA->MODER |= (1U<<3);

	/*****Configure the ADC Module*****/
	//1. Enable clock access to ADC
	RCC->APB2ENR |= ADC1EN;

	//2. Conversion sequence start
	ADC1->SQR3 = ADC_CH1;

	//3. Conversion sequence length
	ADC1->SQR1 = ADC_SEQ_LEN_1;

	//4. Enable ADC module
	ADC1->CR2 |= CR2_ADON;
}

void start_conversion(void) {
	//Start ADC conversion
	ADC1->CR2 |= CR2_SWSTART;
}

uint32_t adc_read(void) {
	//Wait for conversion to be complete
	while(!(ADC1->SR & SR_EOC)); //EOC pin becomes 0 just when conversion is complete

	//Return the converted result
	return (ADC1->DR); //Return the data in data register
}
