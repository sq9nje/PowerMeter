/*
 * usart_2.c
 *
 *  Created on: 16 lip 2016
 *      Author: zelazko
 */
#include "usart_2.h"
#include "hdr_gpio.h"
#include "gpio.h"
#include "itoa.h"
#include "stm32f103xb.h"

#ifdef BUFFERED
// initialize buffers
volatile FIFO_TypeDef U2Rx, U2Tx;
#endif

/*------------------------------------------------------------------------*//**
* \brief Initializes USART2
* \details Initializes USART2 - configures clocks and pin modes, sets the
* baud rate and enables the IRQ.
*
* \param [in] baudRate desired UART baud rate
*//*-------------------------------------------------------------------------*/
void USART2_init(uint32_t baudRate)
{
	#ifdef BUFFERED
	// initialize buffers
	buffer_init(&U2Rx);
	buffer_init(&U2Tx);
	#endif

	// enable bus clocks
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN;
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

	// tx pin mode
	gpio_pin_cfg(GPIOA, 2, GPIO_CRx_MODE_CNF_ALT_PP_2M_value);
	// rx pin mode
	gpio_pin_cfg(GPIOA, 3, GPIO_CRx_MODE_CNF_IN_FLOATING_value);

	USART2->BRR = 64000000/2/baudRate;
	USART2->CR1 =  USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;

	#ifdef BUFFERED
	// enbale USART2 interrupt
	NVIC_EnableIRQ(USART2_IRQn);
	// disable Transmit Data Register empty interrupt
	USART2->CR1 &= ~USART_CR1_TXEIE;
	// enable Receive Data register not empty interrupt
	USART2->CR1 |= USART_CR1_RXNEIE;
	#endif
}

/*------------------------------------------------------------------------*//**
* \brief Send a character
* \details Sends a single character through USART2
*
* \param [in] ch character to be sent
*//*-------------------------------------------------------------------------*/
void USART2_putChar(char ch)
{
	#ifdef BUFFERED
	//put char to the buffer
	buffer_put(&U2Tx, ch);
	//enable Transmit Data Register empty interrupt
	USART2->CR1 |= USART_CR1_TXEIE;
	#else
	// wait for until TX ready
	while(!(USART2->SR & USART_SR_TXE));
	USART2->DR = ch;
	#endif
}

/*------------------------------------------------------------------------*//**
* \brief Send a string
* \details Send a string via USART2
*
* \param [in] s string to be sent
*//*-------------------------------------------------------------------------*/
void USART2_putString(const char* s)
{
	while(*s)
		USART2_putChar(*s++);
}

/*------------------------------------------------------------------------*//**
* \brief Send an integer
* \details Send an integer via USART2. Support different system bases.
*
* \param [in] i integer value
* \param [in] base system base
*//*-------------------------------------------------------------------------*/
void USART2_putInt(int i, int base)
{
	char _buf[16];
	itoa(i, _buf, base);
	USART2_putString(_buf);
}

/*------------------------------------------------------------------------*//**
* \brief Receive a character
* \details Returns a character received via USART2.
*
* \returns char
*//*-------------------------------------------------------------------------*/
char USART2_get(void)
{
#ifdef BUFFERED
	uint8_t ch;
	//check if buffer is empty
	while (buffer_isEmpty(U2Rx) == SUCCESS);
	buffer_get(&U2Rx, &ch);
	return ch;
#else
	 while (!(USART2->SR & USART_SR_RXNE));
		return (char)USART2->DR;
#endif
}

/*------------------------------------------------------------------------*//**
* \brief Send a formatted string
* \details Send a printf formatted string via USART2. Produces large code.
*//*-------------------------------------------------------------------------*/
void USART2_printf(const char * format, ...) {
	char _buf[256];
	va_list arglist;
	va_start(arglist, format);
	vsnprintf(_buf, 256, format, arglist);
	va_end(arglist);
	USART2_putString(_buf);
}

/*------------------------------------------------------------------------*//**
* \brief IRQ handler for USART2
*//*-------------------------------------------------------------------------*/
void USART2_IRQHandler(void)
{
	uint8_t ch;
	//if Receive interrupt
	if (USART2->SR & USART_SR_RXNE)	{
		ch = USART2->DR;
		#ifdef BUFFERED
		//put char to the buffer
		buffer_put(&U2Rx, ch);
		#endif
	}
	if (USART2->SR & USART_SR_TXE)
	{
		#ifdef BUFFERED
		if (buffer_get(&U2Tx, &ch) == SUCCESS)	//if buffer read
			USART2->DR = ch;
		else	//if buffer empty
		#endif
			//disable Transmit Data Register empty interrupt
			USART2->CR1 &= ~USART_CR1_TXEIE;
	}
}
