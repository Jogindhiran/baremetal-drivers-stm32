#include "uart.h"

#define GPIOAEN			(1U<<0)
#define UART2EN			(1U<<17)

#define CR1_TE			(1U<<3)		//Transmitter enable
#define CR1_RE			(1U<<2)		//Receiver enable

#define CR1_UE			(1U<<13)	//USART enable

#define SR_TXE			(1U<<7)		//USART Status Register TX
#define SR_RXNE			(1U<<5)		//USART Status Register RX

#define CR1_RXNEIE		(1U<<5)		//USART RXNE Interrupt Enable

#define DMA1EN						(1U<<21)	//Enable clock access to DMA1
#define CHSEL4						(1U<<27)    //Channel select 4
#define DMA_MEM_INC 				(1U<<10)    //DMA memory increment enable
#define DMA_DIR_MEM_TO_PERIPH		(1U<<6) //Memory to Peripheral direction
#define DMA_CR_TCIE					(1U<<4)		//Transfer complete mode in DMA
#define DMA_CR_EN					(1U<<0)     //DMA stream enable
#define UART_CR3_DMAT				(1U<<7)



#define SYS_FREQ		16000000	//16MHz default
#define APB1_CLK		SYS_FREQ    //USART2 is connected to APB1

#define UART_BAUDRATE			115200		//Baud rate of UART2


static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t BaudRate); 	//Assigns the calculated BRR value to UARTx BRR
static uint16_t compute_uart_bd(uint32_t PeriphClk, uint32_t BaudRate); 						//Calculates BRR Value for UARTx

void uart2_rxtx_init(void);
void uart2_tx_init(void);
char uart2_read(void);
void uart2_write(int ch);
void uart2_rx_interrupt_init(void);

int __io_putchar(int ch) {
	uart2_write(ch);
	return ch;
}

void dma1_stream6_init(uint32_t src, uint32_t dst, uint32_t len) {
	/*Enable clock access to DMA1*/
	RCC->AHB1ENR |= DMA1EN;

	/*Disable DMA1 stream 6*/
	DMA1_Stream6->CR &= ~DMA_CR_EN;

	/*Wait until DMA1 Stream 6 is disabled*/
	while(DMA1_Stream6->CR & DMA_CR_EN) {}

	/*Clear all the interrupt flags of stream 6*/
	DMA1->HIFCR |= (1U<<16);
	DMA1->HIFCR |= (1U<<18);
	DMA1->HIFCR |= (1U<<19);
	DMA1->HIFCR |= (1U<<20);
	DMA1->HIFCR |= (1U<<21);

	/*Set the destination buffer*/
	DMA1_Stream6->PAR = dst; //Peripheral address register

	/*Set the source buffer*/
	DMA1_Stream6->M0AR = src; //Memory 0 address register

	/*Set length*/
	DMA1_Stream6->NDTR = len; //Number of data register

	/*Select Stream 6 Channel 4*/
	DMA1_Stream6->CR = CHSEL4; //Configuration register

	/*Enable memory increment*/
	DMA1_Stream6->CR |= DMA_MEM_INC;

	/*Configure transfer direction*/
	//Memory to peripheral
	DMA1_Stream6->CR |= DMA_DIR_MEM_TO_PERIPH;

	/*Enable DMA transfer complete interrupt*/
	DMA1_Stream6->CR |= DMA_CR_TCIE;

	/*Enable direct mode and disable FIFO*/
	DMA1_Stream6->FCR = 0;

	/*Enable DMA1 stream 6*/
	DMA1_Stream6->CR |= DMA_CR_EN;

	/*Enable UART2 transmitter DMA*/
	USART2->CR3 |= UART_CR3_DMAT;

	/*Enable DMA Interrupt in NVIC*/
	NVIC_EnableIRQ(DMA1_Stream6_IRQn);
}
void uart2_rx_interrupt_init(void) { //Initializes rx interrupt
	/*****Configure UART2 GPIO Pin*****/
	//1. Enable clock access to GPIOA
	RCC->AHB1ENR |= GPIOAEN;

	//TX setup:
	//2. Set PA2 mode to alternate function mode (PA2 is the TX pin for UART2)
	GPIOA->MODER &= ~(1U<<4); //10 for alternate function mode
	GPIOA->MODER |= (1U<<5);

	//3. Set PA2 alternate function type to UART_TX (AF07)
	GPIOA->AFR[0] |= (1U<<8); //0111 for AF7
	GPIOA->AFR[0] |= (1U<<9);
	GPIOA->AFR[0] |= (1U<<10);
	GPIOA->AFR[0] &= ~(1U<<11);

	//RX setup:
	//2. Set PA3 mode to alternate function mode (PA3 is the RX pin for UART2)
	GPIOA->MODER &= ~(1U<<6); //10 for alternate function mode
	GPIOA->MODER |= (1U<<7);

	//3. Set PA3 alternate function type to UART_RX (AF07)
	GPIOA->AFR[0] |= (1U<<12); //0111 for AF7
	GPIOA->AFR[0] |= (1U<<13);
	GPIOA->AFR[0] |= (1U<<14);
	GPIOA->AFR[0] &= ~(1U<<15);

	/*****Configure UART2 module*****/
	//1. Enable clock access to UART2
	RCC->APB1ENR |= UART2EN;

	//2. Configure baudrate
	uart_set_baudrate(USART2, APB1_CLK, UART_BAUDRATE);

	//3. Configure the transfer direction
	USART2->CR1 = (CR1_TE | CR1_RE);

	/*Enable RXNE Interrupt*/
	USART2->CR1 |= CR1_RXNEIE;

	/*Enable UART2 Interrupt in NVIC*/
	NVIC_EnableIRQ(USART2_IRQn);		//In built function

	//4. Enable UART module
	USART2->CR1 |= CR1_UE;

}

void uart2_rxtx_init(void) { //Initializes both rx and tx modes of UART2
	/*****Configure UART2 GPIO Pin*****/
	//1. Enable clock access to GPIOA
	RCC->AHB1ENR |= GPIOAEN;

	//TX setup:
	//2. Set PA2 mode to alternate function mode (PA2 is the TX pin for UART2)
	GPIOA->MODER &= ~(1U<<4); //10 for alternate function mode
	GPIOA->MODER |= (1U<<5);

	//3. Set PA2 alternate function type to UART_TX (AF07)
	GPIOA->AFR[0] |= (1U<<8); //0111 for AF7
	GPIOA->AFR[0] |= (1U<<9);
	GPIOA->AFR[0] |= (1U<<10);
	GPIOA->AFR[0] &= ~(1U<<11);

	//RX setup:
	//2. Set PA3 mode to alternate function mode (PA3 is the RX pin for UART2)
	GPIOA->MODER &= ~(1U<<6); //10 for alternate function mode
	GPIOA->MODER |= (1U<<7);

	//3. Set PA3 alternate function type to UART_RX (AF07)
	GPIOA->AFR[0] |= (1U<<12); //0111 for AF7
	GPIOA->AFR[0] |= (1U<<13);
	GPIOA->AFR[0] |= (1U<<14);
	GPIOA->AFR[0] &= ~(1U<<15);

	/*****Configure UART2 module*****/
	//1. Enable clock access to UART2
	RCC->APB1ENR |= UART2EN;

	//2. Configure baudrate
	uart_set_baudrate(USART2, APB1_CLK, UART_BAUDRATE);

	//3. Configure the transfer direction
	USART2->CR1 = (CR1_TE | CR1_RE);

	//4. Enable UART module
	USART2->CR1 |= CR1_UE;

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

char uart2_read(void) {
	//1. Make sure the receive data register is not empty
	while(!(USART2->SR & SR_RXNE)) {}

	//2. Read Data
	return USART2->DR;
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
