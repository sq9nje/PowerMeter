/*
 * usart_2.h
 *
 *  Created on: 16 lip 2016
 *      Author: zelazko
 */

#ifndef USART_2_H_
#define USART_2_H_

#include <stdint.h>
#include "buffer.h"

#define BUFFERED

#ifdef BUFFERED
extern volatile FIFO_TypeDef U2Rx, U2Tx;
#endif

void USART2_init(uint32_t baudRate);
void USART2_putChar(char ch);
void USART2_putString(const char* s);
void USART2_putInt(int i, int base);
char USART2_get(void);

#ifdef USE_SPRINTF
#include <stdio.h>
#include <stdarg.h>
void USART2_printf(const char * format, ...);
#endif

void USART2_IRQHandler(void);

#endif /* USART_2_H_ */
