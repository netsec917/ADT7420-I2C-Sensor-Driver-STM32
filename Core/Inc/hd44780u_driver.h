/*
 * hd44780u_driver.h
 *
 *  Created on: Aug 16, 2020
 *      Author: Tom
 */

#ifndef INC_HD44780U_DRIVER_H_
#define INC_HD44780U_DRIVER_H_

#include "main.h"
#include "stdint.h"
#include "stddef.h"
#include "stdbool.h"

// HD44780U INSTRUCTION SET
#define HD44780U_DISPLAY_CLEAR (uint8_t)0x1U
#define HD44780U_RETURN_HOME (uint8_t)0x2U
#define HD44780U_ENTRY_MODE_SET (uint8_t)0x4U
#define HD44780U_DISPLAY_CTRL (uint8_t)0x8U
#define HD44780U_SHIFT_CTRL (uint8_t)0x10U
#define HD47780U_FUNCTION_SET (uint8_t)0x20U
#define HD44780U_SET_CGRAM_ADDR (uint8_t)0x40U
#define HD44780U_SET_DDRAM_ADDR (uint8_t)0x80U

// *** HD44780U INSTRUCTION FLAGS ***

// HD44780U ENTRY MODE SET FLAGS
#define HD44780U_ENTRY_MODE_INC (uint8_t)0x2U
#define HD44780U_ENTRY_MODE_DEC (uint8_t)0x0U
#define HD44780U_ENTRY_MODE_SHIFT (uint8_t)0x1U

// HD44780U DISPLAY CONTROL FLAGS
#define HD44780U_DISPLAY_ON (uint8_t)0x4U
#define HD44780U_DISPLAY_OFF (uint8_t)0x0U
#define HD44780U_CURSOR_ON (uint8_t)0x2U
#define HD44780U_CURSOR_OFF (uint8_t)0x0U
#define HD44780U_BLINK_ON (uint8_t)0x1U
#define HD44780U_BLINK_OFF (uint8_t)0x0U

// HD44780U SHIFT CONTROL FLAGS
#define HD44780U_CURSOR_SHIFT (uint8_t)0x0U
#define HD44780U_DISPLAY_SHIFT (uint8_t)0x8U
#define HD44780U_SHIFT_LEFT (uint8_t)0x0U
#define HD44780U_SHIFT_RIGHT (uint8_t)0x4U

// HD44780U FUNCTION SET FLAGS
#define HD44780U_5x8_CHAR_FONT (uint8_t)0x0U
#define HD44780U_5x10_CHAR_FONT (uint8_t)0x4U
#define HD44780U_1_DISPLAY_LINES (uint8_t)0x0U
#define HD44780U_2_DISPLAY_LINES (uint8_t)0x8U
#define HD44780U_4_BIT_INTERFACE (uint8_t)0x0U
#define HD44780U_8_BIT_INTERFACE (uint8_t)0x10U

// DDRAM & CGRAM Addresses
#define HD44780U_MIN_CGRAM_ADDR (uint8_t)0x0U
#define HD44780U_MAX_CGRAM_ADDR (uint8_t)0xFFU
#define HD44780U_MIN_DDRAM_ADDR (uint8_t)0x0U
#define HD44780U_MAX_DDRAM_ADDR (uint8_t)0x67U

// Display position constants
#define HD44780U_MAX_ROW_POS (uint8_t)0x1U
#define HD44780U_MAX_COL_POS (uint8_t)0xFU


// Typedefs
typedef enum {
	HD44780U_OK,
	HD44780U_INVALID_FLAGS,
	HD44780U_INVALID_DISPLAY_POS
} Hd44780u_status;

typedef struct {
	uint8_t row;
	uint8_t col;
} hd44780u_cursor;

typedef struct {
	GPIO_TypeDef* port;
	uint32_t en_pin;
	uint32_t rs_pin;
	uint32_t d4_pin;
	uint32_t d5_pin;
	uint32_t d6_pin;
	uint32_t d7_pin;
	hd44780u_cursor cursor;
	uint8_t display_status;
} hd44780u;

// Function prototypes
void hd44780u_init(hd44780u* display);
void hd44780u_write_nibble(hd44780u* display, uint8_t nibble);
void hd44780u_write_command(hd44780u* display, uint8_t command);
void hd44780u_write_data(hd44780u* display, uint8_t addr);
Hd44780u_status hd44780u_display_on(hd44780u* display, uint8_t cursor_flags);
void hd44780u_display_off(hd44780u* display);
void hd44780u_display_clear(hd44780u* display);
void hd44780u_display_home(hd44780u* display);
void hd44780u_cursor_on(hd44780u* display);
void hd44780u_cursor_off(hd44780u* display);
void hd44780u_blink_on(hd44780u* display);
void hd44780u_blink_off(hd44780u* display);
Hd44780u_status hd44780u_shift_cursor(hd44780u* display, uint8_t direction);
Hd44780u_status hd44780u_set_cursor(hd44780u* display, uint8_t row, uint8_t col);
Hd44780u_status hd44780u_put_char(hd44780u* display, uint8_t c);
Hd44780u_status hd44780u_put_str(hd44780u* display, char *str, size_t len);
#endif /* INC_HD44780U_DRIVER_H_ */
