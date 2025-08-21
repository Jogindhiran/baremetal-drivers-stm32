#include <stdint.h>
#include "stm32f4xx.h"

//

//Symbolic names:
#define GPIOAEN			(1U<<0)
#define UART2EN			(1U<<17)

#define CR1_TE			(1U<<3)		//Transmitter enable
#define CR1_UE			(1U<<13)	//USART enable
#define SR_TXE			(1U<<7)		//USART Status Register TX

#define SYS_FREQ		16000000	//16MHz default
#define APB1_CLK		SYS_FREQ    //USART2 is connected to APB1

#define UART_BAUDRATE			115200		//Baud rate of UART2

//Functions:
static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t BaudRate); 	//Assigns the calculated BRR value to UARTx BRR
static uint16_t compute_uart_bd(uint32_t PeriphClk, uint32_t BaudRate); 						//Calculates BRR Value for UARTx

void uart2_tx_init(void);																		//Initializes USART2
void uart2_write(int ch);

int main(void) {

	uart2_tx_init();

	while(1) {
		uart2_write('Y');
	}
}

void uart2_tx_init(void) {
	/*****Configure UART2 GPIO Pin*****/
	//1. Enable clock access to GPIOA
	RCC->AHB1ENR |= GPIOAEN;

	//2. Set PA2 mode to alternate function mode (PA2 is the TX pin for UART2)
	GPIOA->MODER &= ~(1U<<4); //10 for alternate function mode
	GPIOA->MODER |= (1U<<5);

	//3. Set PA2 alternate function type to UART_TX (AF07)
	GPIOA->AFR[0] |= (1U<<8); //0111 for AF7
	GPIOA->AFR[0] |= (1U<<9);
	GPIOA->AFR[0] |= (1U<<10);
	GPIOA->AFR[0] &= ~(1U<<11);

	/*****Configure UART2 module*****/
	//1. Enable clock access to UART2
	RCC->APB1ENR |= UART2EN;

	//2. Configure baudrate
	uart_set_baudrate(USART2, APB1_CLK, UART_BAUDRATE);
	//3. Configure the transfer direction
	USART2->CR1 = CR1_TE;
	//4. Enable UART module
	USART2->CR1 |= CR1_UE;

}

void uart2_write(int ch) {
	//1. Make sure the transmit data register is empty (using status register)
	while(!(USART2->SR & SR_TXE)); //while result is !(0), stop here

	//2. Write to transmit data register (DR)
	USART2->DR = (ch & 0xFF); // & with 0xFF to make the data as 8 bit
}

static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t BaudRate) {
	USARTx->BRR = compute_uart_bd(PeriphClk,BaudRate); //BRR - Baud Rate Register
}

static uint16_t compute_uart_bd(uint32_t PeriphClk, uint32_t BaudRate) {
	return ((PeriphClk + BaudRate/2U)/BaudRate); //Has to be stored in UART BaudRate Register
}
