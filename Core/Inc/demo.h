/*
 * demo.h
 *
 *  Created on: Aug 23, 2020
 *      Author: Tom
 */

#include "main.h"
#include "adt7420_driver.h"
#include "ring_buffer.h"
#include "stdbool.h"
#include "hd44780u_driver.h"

// Enter sleep mode with wake from interrupt, and keep flash on
#define SLEEP_MODE() {\
	SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;\
	FLASH->ACR &= ~FLASH_ACR_SLEEP_PD; \
	__WFI(); \
}

extern ring_buffer usart_tx_buf;
extern volatile bool timer2_overflow_flag;

void sys_init(void);
void hd44780u_config(void);
void adt7420_config(void);
void read_adt7420(void);
void usart_log_temperature(char* str);
