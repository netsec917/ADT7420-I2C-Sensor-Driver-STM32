/*
 * ring_buffer.c
 *
 *  Created on: Sep 4, 2020
 *      Author: Tom
 */

#include "ring_buffer.h"

extern inline uint8_t ring_buffer_mask(ring_buffer* buf, uint8_t val);
extern inline void ring_buffer_init(ring_buffer* buf);
extern inline void ring_buffer_enqueue(ring_buffer* buf, uint8_t data);
extern inline uint8_t ring_buffer_dequeue(ring_buffer* buf);
extern inline uint8_t ring_buffer_size(ring_buffer* buf);
extern inline bool ring_buffer_empty(ring_buffer* buf);
extern inline bool ring_buffer_full(ring_buffer* buf);
