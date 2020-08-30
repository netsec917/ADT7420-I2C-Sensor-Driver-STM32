/*
 * ring_buffer.h
 *
 *  Created on: Aug 26, 2020
 *      Author: Tom
 */

#ifndef INC_RING_BUFFER_H_
#define INC_RING_BUFFER_H_

#include "stdint.h"
#include "stdbool.h"
#include "assert.h"
#include "string.h"

#define BUF_SIZE 128U // Must be a power of 2 for bitwise & masking to work

typedef struct {
	uint8_t read;
	uint8_t write;
	uint8_t buffer[BUF_SIZE];
}ring_buffer;

void ring_buffer_init(ring_buffer* buf);
void ring_buffer_enqueue(ring_buffer* buf, uint8_t data);
uint8_t ring_buffer_dequeue(ring_buffer* buf);
uint8_t ring_buffer_size(ring_buffer* buf);
bool ring_buffer_empty(ring_buffer* buf);
bool ring_buffer_full(ring_buffer* buf);

#endif /* INC_RING_BUFFER_H_ */
