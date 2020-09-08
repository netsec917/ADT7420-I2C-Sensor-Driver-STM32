/*
 * ring_buffer.h
 *
 *  Created on: Sep 3, 2020
 *      Author: Tom
 */

#ifndef RING_BUFFER_H_
#define RING_BUFFER_H_

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#define RING_BUFFER_SIZE 16U // Must be a power of 2 for bitwise & masking to work

typedef struct {
	volatile uint8_t read;
	volatile uint8_t write;
	volatile uint8_t buffer[RING_BUFFER_SIZE];
} ring_buffer;

static inline uint8_t ring_buffer_mask(ring_buffer* buf, uint8_t val);
static inline void ring_buffer_init(ring_buffer* buf);
static inline void ring_buffer_enqueue(ring_buffer* buf, uint8_t data);
static inline uint8_t ring_buffer_dequeue(ring_buffer* buf);
static inline uint8_t ring_buffer_size(ring_buffer* buf);
static inline bool ring_buffer_empty(ring_buffer* buf);
static inline bool ring_buffer_full(ring_buffer* buf);


static inline void ring_buffer_init(ring_buffer* buf)
{
	buf->read = 0;
	buf->write = 0;
	memset(buf, 0, RING_BUFFER_SIZE);
}

static inline uint8_t ring_buffer_mask(ring_buffer* buf, uint8_t val)
{
	return val & (RING_BUFFER_SIZE - 1);
}

static inline void ring_buffer_enqueue(ring_buffer* buf, uint8_t data)
{
	assert(!ring_buffer_full(buf));
	buf->buffer[ring_buffer_mask(buf, buf->write++)] = data;
}

static inline uint8_t ring_buffer_dequeue(ring_buffer* buf)
{
	assert(!ring_buffer_empty(buf));
	return buf->buffer[ring_buffer_mask(buf, buf->read++)];
}

static inline uint8_t ring_buffer_size(ring_buffer* buf)
{
	return buf->write - buf->read;
}

static inline bool ring_buffer_empty(ring_buffer* buf)
{
	return buf->read == buf->write;
}

static inline bool ring_buffer_full(ring_buffer* buf)
{
	return ring_buffer_size(buf) == RING_BUFFER_SIZE;
}

#endif
