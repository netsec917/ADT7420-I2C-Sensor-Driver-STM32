/*
 * demo.c
 *
 *  Created on: Aug 26, 2020
 *      Author: Tom
 */
#include "demo.h"
#include "string.h"
#include "stdio.h"

ring_buffer usart_tx_buf; // TODO may need to make volatile, or underlying struct members?
volatile bool timer2_overflow_flag = false;

static adt7420_dev dev;
static hd44780u display;
static char str_buf[16];
static char lcd_buf[16];

void hd44780u_config(void)
{
	display.port = GPIOB;
	display.en_pin = LL_GPIO_PIN_4;
	display.rs_pin = LL_GPIO_PIN_5;
	display.d4_pin = LL_GPIO_PIN_0;
	display.d5_pin = LL_GPIO_PIN_7;
	display.d6_pin = LL_GPIO_PIN_6;
	display.d7_pin = LL_GPIO_PIN_1;
	hd44780u_init(&display);
	hd44780u_display_on(&display, HD44780U_CURSOR_OFF | HD44780U_BLINK_OFF);
}

void adt7420_config(void)
{
	adt7420_settings params;
	params.config = ADT7420_16_BIT_RES | ADT7420_COMP_MODE | ADT7420_FAULT_QUEUE_1 | ADT7420_INT_ACTIVE_HIGH | ADT7420_CT_ACTIVE_HIGH | ADT7420_CONTINUOUS_MODE;
	params.crit_temperature_c = 30;
	params.high_temperature_c = 27;
	params.low_temperature_c = 18;
	params.hysteresis = 2;

	dev.i2c_addr = 0x4B; // Jumper 1 & 2 Open
	dev.i2c_ch = I2C1;
	adt7420_init(&dev, &params);
}

void usart_log_temperature(char *str)
{
	while (*str != '\0') {
		if (!ring_buffer_full(&usart_tx_buf)) {
			ring_buffer_enqueue(&usart_tx_buf, *str);
			++str;
		} else {
			break; // Nothing else to do but break and discard the rest of the string
		}
	}
	// (R)Enable TXE interrupt so the buffer will be emptied by the ISR in the background
	LL_USART_EnableIT_TXE(USART2);
}

void read_adt7420(void)
{
	float temperature;
	adt7420_get_temperature(&dev, &temperature);
	sprintf(str_buf, "Temp: %dC\n\r", (int)temperature);
	sprintf(lcd_buf, "Temp: %dC", (int)temperature);
	usart_log_temperature(str_buf);
	hd44780u_display_home(&display);
	hd44780u_put_str(&display, lcd_buf, strlen(lcd_buf));
}

void sys_init(void)
{
	ring_buffer_init(&usart_tx_buf);
	hd44780u_config();
	adt7420_config();
}