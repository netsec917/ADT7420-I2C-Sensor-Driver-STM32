/*
 * hd44780u_driver.c
 *
 *  Created on: Aug 16, 2020
 *      Author: Tom
 */

#include "hd44780u_driver.h"

static inline void hd44780u_pulse_en(hd44780u* display);

static inline void hd44780u_pulse_en(hd44780u* display)
{
	display->port->BSRR = display->en_pin;
	LL_mDelay(1);
	display->port->BRR = display->en_pin;
}

void hd44780u_init(hd44780u* display)
{
	// 8 Bit-mode function set instructions
	LL_mDelay(100); // Todo: See if delay can be reduced without issue
	hd44780u_write_nibble(display, 0x3U);
	LL_mDelay(4);
	hd44780u_write_nibble(display, 0x3U);
	LL_mDelay(1);
	hd44780u_write_nibble(display, 0x3U);
	LL_mDelay(1);
	hd44780u_write_nibble(display, 0x2U);
	LL_mDelay(1);

	// DISPLAY NOW IN 4-BIT MODE
	hd44780u_write_command(display, 0x2U); // Real function set: 2 Lines & 5x8 font
	hd44780u_write_command(display, HD44780U_DISPLAY_CTRL | HD44780U_DISPLAY_OFF);
	LL_mDelay(1);

	hd44780u_write_command(display, HD44780U_DISPLAY_CLEAR);
	LL_mDelay(3);
	// Set address counter to increment after ddram write
	hd44780u_write_command(display, HD44780U_ENTRY_MODE_SET | HD44780U_ENTRY_MODE_INC);
	LL_mDelay(1);
}


void hd44780u_write_nibble(hd44780u* display, uint8_t nibble)
{
	if (nibble & 0x1U) {
		display->port->BSRR = display->d4_pin;
	} else {
		display->port->BRR = display->d4_pin;
	}
	if (nibble & 0x2U) {
		display->port->BSRR = display->d5_pin;
	} else {
		display->port->BRR = display->d5_pin;
	}
	if (nibble & 0x4U) {
		display->port->BSRR = display->d6_pin;
	} else {
		display->port->BRR = display->d6_pin;
	}
	if (nibble & 0x8U) {
		display->port->BSRR = display->d7_pin;
	} else {
		display->port->BRR = display->d7_pin;
	}
}

void hd44780u_write_command(hd44780u* display, uint8_t command)
{
	display->port->BRR = display->rs_pin; // RS pin low to select instruction register
	hd44780u_write_nibble(display, command >> 4U); // Shift upper nibble to lower bits for first write
	hd44780u_pulse_en(display);
	hd44780u_write_nibble(display, command & 0xFU); // Now we only care about the lower nibble
	hd44780u_pulse_en(display);
}

void hd44780u_write_data(hd44780u* display, uint8_t addr)
{
	display->port->BSRR = display->rs_pin;
	hd44780u_write_nibble(display, addr >> 4U);
	hd44780u_pulse_en(display);
	hd44780u_write_nibble(display, addr & 0xFU);
	hd44780u_pulse_en(display);
}

Hd44780u_status hd44780u_display_on(hd44780u* display, uint8_t cursor_flags)
{
	if (cursor_flags > (HD44780U_CURSOR_ON | HD44780U_BLINK_ON)) {
		return HD44780U_INVALID_FLAGS;
	}

	display->cursor.row = 0;
	display->cursor.col = 0;
	display->display_status = HD44780U_DISPLAY_ON | cursor_flags;
	hd44780u_write_command(display, HD44780U_DISPLAY_CTRL | display->display_status);
	return HD44780U_OK;
}

void hd44780u_display_off(hd44780u* display)
{
	display->display_status = HD44780U_DISPLAY_OFF;
	hd44780u_write_command(display, HD44780U_DISPLAY_CTRL | display->display_status);
}

void hd44780u_display_clear(hd44780u* display)
{
	display->cursor.row = 0;
	display->cursor.col = 0;
	hd44780u_write_command(display, HD44780U_DISPLAY_CLEAR);
}

void hd44780u_display_home(hd44780u* display)
{
	display->cursor.row = 0;
	display->cursor.col = 0;
	hd44780u_write_command(display, HD44780U_RETURN_HOME);
}

void hd44780u_cursor_on(hd44780u* display)
{
	display->display_status = HD44780U_DISPLAY_ON | HD44780U_CURSOR_ON;
	hd44780u_write_command(display, HD44780U_DISPLAY_CTRL | display->display_status);
}

void hd44780u_cursor_off(hd44780u* display)
{
	display->display_status = HD44780U_DISPLAY_ON | HD44780U_CURSOR_OFF;
	hd44780u_write_command(display, HD44780U_DISPLAY_CTRL | display->display_status);
}

void hd44780u_blink_on(hd44780u* display)
{
	display->display_status = HD44780U_DISPLAY_ON | HD44780U_CURSOR_ON | HD44780U_BLINK_ON;
	hd44780u_write_command(display, HD44780U_DISPLAY_CTRL | display->display_status);
}

void hd44780u_blink_off(hd44780u* display)
{
	display->display_status = HD44780U_DISPLAY_ON | HD44780U_CURSOR_ON | HD44780U_BLINK_OFF;
	hd44780u_write_command(display, HD44780U_DISPLAY_CTRL | display->display_status);
}

Hd44780u_status hd44780u_shift_cursor(hd44780u* display, uint8_t direction)
{
	if (direction != HD44780U_SHIFT_LEFT && direction != HD44780U_SHIFT_RIGHT) {
		return HD44780U_INVALID_FLAGS;
	}

	if ((direction == HD44780U_SHIFT_LEFT && display->cursor.col == 0U)
	|| (direction == HD44780U_SHIFT_RIGHT && display->cursor.col == HD44780U_MAX_COL_POS)) {
		return HD44780U_INVALID_DISPLAY_POS;
	}

	if (direction == HD44780U_SHIFT_LEFT) {
		--display->cursor.col;
	} else {
		++display->cursor.col;
	}

	hd44780u_write_command(display, HD44780U_SHIFT_CTRL | HD44780U_CURSOR_SHIFT | direction);
	return HD44780U_OK;
}

Hd44780u_status hd44780u_set_cursor(hd44780u* display, uint8_t row, uint8_t col)
{
	if (row > HD44780U_MAX_ROW_POS || col > HD44780U_MAX_COL_POS) {
		return HD44780U_INVALID_DISPLAY_POS;
	}

	display->cursor.row = row;
	display->cursor.col = col;

	if (display->cursor.row == 0) {
		hd44780u_write_command(display, HD44780U_SET_DDRAM_ADDR | display->cursor.col);
	} else {
		//0x40U == DDRAM row 1 offset
		hd44780u_write_command(display, HD44780U_SET_DDRAM_ADDR | (display->cursor.col + 0x40U));
	}
	return HD44780U_OK;
}

Hd44780u_status hd44780u_put_char(hd44780u* display, uint8_t c)
{
	if (display->cursor.col > HD44780U_MAX_COL_POS) {
		return HD44780U_INVALID_DISPLAY_POS;
	}
	
	hd44780u_write_data(display, c);
	++display->cursor.col;
	return HD44780U_OK;
}

Hd44780u_status hd44780u_put_str(hd44780u* display, char* str, size_t len)
{
	// + 1 to account for 0-based ddram addressing
	if (display->cursor.col + len > HD44780U_MAX_COL_POS + 1U) {
		return HD44780U_INVALID_DISPLAY_POS;
	}

	while (*str != '\0') {
		hd44780u_put_char(display, *str);
		++str;
	}
	return HD44780U_OK;
}
