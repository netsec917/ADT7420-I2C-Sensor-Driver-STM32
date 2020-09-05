/*
 * ring_buffer.h
 *
 *  Created on: Sep 3, 2020
 *      Author: Tom
 */

#ifndef RING_BUFFER_H_
#define RING_BUFFER_H_

#include "stdint.h"
#include "stdbool.h"
#include "assert.h"
#include "string.h"

#define BUF_SIZE 16U // Must be a power of 2 for bitwise & masking to work

typedef struct {
	volatile uint8_t read;
	volatile uint8_t write;
	volatile uint8_t buffer[BUF_SIZE];
} ring_buffer;

inline uint8_t ring_buffer_mask(ring_buffer* buf, uint8_t val);
inline void ring_buffer_init(ring_buffer* buf);
inline void ring_buffer_enqueue(ring_buffer* buf, uint8_t data);
inline uint8_t ring_buffer_dequeue(ring_buffer* buf);
inline uint8_t ring_buffer_size(ring_buffer* buf);
inline bool ring_buffer_empty(ring_buffer* buf);
inline bool ring_buffer_full(ring_buffer* buf);


inline void ring_buffer_init(ring_buffer* buf)
{
	buf->read = 0;
	buf->write = 0;
	memset(buf, 0, BUF_SIZE);
}

inline uint8_t ring_buffer_mask(ring_buffer* buf, uint8_t val)
{
	return val & (BUF_SIZE - 1);
}

inline void ring_buffer_enqueue(ring_buffer* buf, uint8_t data)
{
	assert(!ring_buffer_full(buf));
	buf->buffer[ring_buffer_mask(buf, buf->write++)] = data;
}

inline uint8_t ring_buffer_dequeue(ring_buffer* buf)
{
	assert(!ring_buffer_empty(buf));
	return buf->buffer[ring_buffer_mask(buf, buf->read++)];
}

inline uint8_t ring_buffer_size(ring_buffer* buf)
{
	return buf->write - buf->read;
}

inline bool ring_buffer_empty(ring_buffer* buf)
{
	return buf->read == buf->write;
}

inline bool ring_buffer_full(ring_buffer* buf)
{
	return ring_buffer_size(buf) == BUF_SIZE;
}

#endif
