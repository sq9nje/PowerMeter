/*
 * main.c
 *
 *  Created on: 16 lip 2016
 *      Author: zelazko
 */
#include <stdlib.h>
#include "stm32f103xb.h"
#include "hdr_gpio.h"
#include "gpio.h"
#include "bitband.h"
#include "usart_2.h"


#define LED_bb BB(GPIOA->ODR, GPIO_ODR_ODR5)

volatile uint32_t delayVar;
volatile uint32_t ledVar;

void delay(uint32_t cnt){
	delayVar = cnt;
	while(delayVar);
}

/*------------------------------------------------------------------------*//**
* \brief Configures Flash latency
* \details Configures Flash latency (wait-states) which allows the chip to run
* at higher speeds
*
* \param [in] frequency defines the target frequency of the core
*//*-------------------------------------------------------------------------*/
static void flash_latency(uint32_t frequency)
{
	uint32_t wait_states;

	if (frequency < 24000000ul)				// 0 wait states for core speed below 24MHz
		wait_states = 0;
	else if (frequency < 48000000ul)		// 1 wait state for core speed between 24MHz and 48MHz
		wait_states = 1;
	else									// 2 wait states for core speed over 48MHz
		wait_states = 2;

	FLASH->ACR |= wait_states;				// set the latency
}

/*------------------------------------------------------------------------*//**
* \brief Configures System Clocks
* \details Configures system clocks to use HSI as a PLL source.
*    SYSCLK = 64 MHz
*    HCLK = 64 MHz
*    PCLK1 = 32 MHz
*    PCLK2 = 64 MHz
*    ADCCLK = 10.67 MHz
*//*-------------------------------------------------------------------------*/
void SystemInit(void) {
	RCC->CR |= RCC_CR_HSION;
	RCC->CFGR = RCC_CFGR_PLLMULL16 | RCC_CFGR_ADCPRE_DIV6 | RCC_CFGR_PPRE1_DIV2;
	while (!(RCC->CR & RCC_CR_HSIRDY));
	RCC->CR |= RCC_CR_PLLON;
	flash_latency(64000000ul);
	while (!(RCC->CR & RCC_CR_PLLRDY));
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	while ( (RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}

int main(void) {
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	gpio_pin_cfg(GPIOA, 5, GPIO_CRx_MODE_CNF_OUT_PP_2M_value);

	SysTick_Config(64000);

	USART2_init(9600);
	USART2_putString("USART2 test\r\n");

	static volatile uint16_t wyniki[2];
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN | RCC_APB2ENR_ADC1EN;
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
	gpio_pin_cfg(GPIOC, 0, GPIO_CRx_MODE_CNF_IN_ANALOG_value);
	gpio_pin_cfg(GPIOC, 1, GPIO_CRx_MODE_CNF_IN_ANALOG_value);

	DMA1_Channel1->CPAR = (uint32_t)&ADC1->DR;
	DMA1_Channel1->CMAR = (uint32_t)wyniki;
	DMA1_Channel1->CNDTR = 2;
	DMA1_Channel1->CCR = DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_1 | DMA_CCR_MINC | DMA_CCR_EN;

	ADC1->CR2 = ADC_CR2_ADON | ADC_CR2_DMA;
	for(volatile uint32_t delay = 100000; delay; delay--);

	ADC1->CR1 = ADC_CR1_SCAN;
	ADC1->SQR3 = 10 | 11<<5;
	ADC1->SQR1 = ADC_SQR1_L_0;
	BB(ADC1->CR2, ADC_CR2_ADON) = 1;

	while( (DMA1->ISR & DMA_ISR_TCIF1) == 0 );
	USART2_putInt(wyniki[0], 10);
	USART2_putString("\r\n");
	USART2_putInt(wyniki[1], 10);
	USART2_putString("\r\n");

	while(1) {
		USART2_putChar(USART2_get());
	}
}


__attribute__((interrupt)) void SysTick_Handler(void){
	++ledVar;
	if(ledVar >= 500)
	{
		LED_bb ^= 1;
		ledVar = 0;
	}

	if(delayVar)
		--delayVar;
}
