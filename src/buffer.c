/*
 * buffer.c
 *
 *  Created on: 16 lip 2016
 *      Author: zelazko
 */
#include "buffer.h"

/*------------------------------------------------------------------------*//**
* \brief Initializes the buffer
* \details Zeroes the in and out indices and character count.
*//*-------------------------------------------------------------------------*/
void buffer_init(volatile FIFO_TypeDef *buffer)
{
	buffer->count = 0;	//0 bytes in buffer
	buffer->in = 0;		//index points to start
	buffer->out = 0;	//index points to start
}

/*------------------------------------------------------------------------*//**
* \brief Puts a character in the buffer
* \details Puts a character into the buffer and increments the in index.
*//*-------------------------------------------------------------------------*/
ErrorStatus buffer_put(volatile FIFO_TypeDef *buffer, uint8_t ch)
{
	if(buffer->count == USARTBUFFSIZE)
		return ERROR; 		//buffer full
	buffer->buff[buffer->in++] = ch;
	buffer->count++;
	if(buffer->in == USARTBUFFSIZE)
		buffer->in = 0; 	//start from beginning
	return SUCCESS;
}

/*------------------------------------------------------------------------*//**
* \brief Gets a character from the buffer
* \details Fetches a character from the buffer and increments the out index.
*//*-------------------------------------------------------------------------*/
ErrorStatus buffer_get(volatile FIFO_TypeDef *buffer, uint8_t *ch)
{
	if(buffer->count == 0)
		return ERROR; 		//buffer empty
	*ch = buffer->buff[buffer->out++];
	buffer->count--;
	if(buffer->out == USARTBUFFSIZE)
		buffer->out = 0;	//start from beginning
	return SUCCESS;
}

/*------------------------------------------------------------------------*//**
* \brief Checks if the buffer is empty
*//*-------------------------------------------------------------------------*/
ErrorStatus buffer_isEmpty(volatile FIFO_TypeDef buffer)
{
	if(buffer.count == 0)
		return SUCCESS;		//buffer full
	return ERROR;
}
