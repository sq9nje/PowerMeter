/*
 * buffer.c
 *
 *  Created on: 16 lip 2016
 *      Author: zelazko
 */
#include "buffer.h"

void buffer_init(volatile FIFO_TypeDef *buffer)
{
	buffer->count = 0;	//0 bytes in buffer
	buffer->in = 0;		//index points to start
	buffer->out = 0;	//index points to start
}

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

ErrorStatus buffer_isEmpty(volatile FIFO_TypeDef buffer)
{
	if(buffer.count == 0)
		return SUCCESS;		//buffer full
	return ERROR;
}
