/*
 * buffer.h
 *
 *  Created on: 16 lip 2016
 *      Author: zelazko
 */

#ifndef BUFFER_H_
#define BUFFER_H_

#include <stdint.h>

#define USARTBUFFSIZE	128

typedef enum {ERROR = 0, SUCCESS = !ERROR} ErrorStatus;

typedef struct{
	uint8_t in;
	uint8_t out;
	uint8_t count;
	uint8_t buff[USARTBUFFSIZE];
}FIFO_TypeDef;

void buffer_init(volatile FIFO_TypeDef *buffer);
ErrorStatus buffer_put(volatile FIFO_TypeDef *buffer, uint8_t ch);
ErrorStatus buffer_get(volatile FIFO_TypeDef *buffer, uint8_t *ch);
ErrorStatus buffer_isEmpty(volatile FIFO_TypeDef buffer);

#endif /* BUFFER_H_ */
