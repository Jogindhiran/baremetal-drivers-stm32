/*
 * uart.h
 *
 *  Created on: May 5, 2025
 *      Author: Jogindhiran
 */

#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include "stm32f4xx.h"

void uart2_rxtx_init(void);
void uart2_tx_init(void);
char uart2_read(void);
void uart2_write(int ch);
void uart2_rx_interrupt_init(void);
void dma1_stream6_init(uint32_t src, uint32_t dst, uint32_t len);

#define SR_RXNE			(1U<<5)		//USART Status Register RX
#define HISR_TCIF6		(1U<<21)	//Transfer complete bit
#define HIFCR_CTCIF6	(1U<<21)	//For clearing transfer complete flag

#endif /* UART_H_ */
