/*
 * ring_buffer.c
 *
 *  Created on: Aug 26, 2020
 *      Author: Tom
 */


#include "ring_buffer.h"

static inline uint8_t ring_buffer_mask(ring_buffer* buf, uint8_t val);

void ring_buffer_init(ring_buffer* buf)
{
	buf->read = 0;
	buf->write = 0;
	memset(buf, 0, BUF_SIZE);
}

static inline uint8_t ring_buffer_mask(ring_buffer* buf, uint8_t val)
{
	return val & (BUF_SIZE - 1);
}

void ring_buffer_enqueue(ring_buffer* buf, uint8_t data)
{
	assert(!ring_buffer_full(buf));
	buf->buffer[ring_buffer_mask(buf, buf->write++)] = data;
}

uint8_t ring_buffer_dequeue(ring_buffer* buf)
{
	assert(!ring_buffer_empty(buf));
	return buf->buffer[ring_buffer_mask(buf, buf->read++)];
}

uint8_t ring_buffer_size(ring_buffer* buf)
{
	return buf->write - buf->read;
}

bool ring_buffer_empty(ring_buffer* buf)
{
	return buf->read == buf->write;
}

bool ring_buffer_full(ring_buffer* buf)
{
	return ring_buffer_size(buf) == BUF_SIZE;
}
