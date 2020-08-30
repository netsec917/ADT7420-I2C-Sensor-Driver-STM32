/*
 * adt7420_driver.h
 *
 *  Created on: Aug 23, 2020
 *      Author: Tom
 */

#ifndef ADT7420_DRIVER_H_
#define ADT7420_DRIVER_H_

#include "main.h"
#include "stdint.h"


#define ADT7420_CHIP_ID (uint8_t)0xCB
#define ADT7420_REG_SIZE (uint8_t)1U

#define ADT7420_MIN_TEMPERATURE_C (int16_t)-40
#define ADT7420_MAX_TEMPERATURE_C (int16_t)150
#define ADT7420_MIN_HYSTERESIS_C (int16_t)0
#define ADT7420_MAX_HYSTERESIS_C (int16_t)15
#define ADT7420_CELS_TO_FAHR(celsius) ((celsius * (9 / 5)) + 32)
#define ADT7420_FAHR_TO_CELS(fahr) ((fahr - 32) * (9 / 5))

#define ADT7420_RESET (uint8_t)0x2F
#define ADT7420_STATUS (uint8_t)0x2U
#define ADT7420_CONFIG (uint8_t)0x3U
#define ADT7420_ID (uint8_t)0x0BU

#define ADT7420_TEMPERATURE_MSB (uint8_t)0x0U
#define ADT7420_TEMPERATURE_LSB (uint8_t)0x1U

#define ADT7420_TEMPERATURE_CRIT_MSB (uint8_t)0x8U
#define ADT7420_TEMPERATURE_CRIT_LSB (uint8_t)0x9U

#define ADT7420_TEMPERATURE_HIGH_MSB (uint8_t)0x4U
#define ADT7420_TEMPERATURE_HIGH_LSB (uint8_t)0x5U

#define ADT7420_TEMPERATURE_LOW_MSB (uint8_t)0x6U
#define ADT7420_TEMPERATURE_LOW_LSB (uint8_t)0x7U

#define ADT7420_HYSTERESIS (uint8_t)0x0AU

// Config register flag bits
#define ADT7420_FAULT_QUEUE_1 (uint8_t)0x1U
#define ADT7420_FAULT_QUEUE_2 (uint8_t)0x2U
#define ADT7420_FAULT_QUEUE_3 (uint8_t)0x3U
#define ADT7420_FAULT_QUEUE_4 (uint8_t)0x4U

#define ADT7420_CT_ACTIVE_LOW (uint8_t)0x0U
#define ADT7420_CT_ACTIVE_HIGH (uint8_t)0x8U
#define ADT7420_INT_ACTIVE_LOW (uint8_t)0x0U
#define ADT7420_INT_ACTIVE_HIGH (uint8_t)0x10U

#define ADT7420_INT_MODE (uint8_t)0x0U
#define ADT7420_COMP_MODE (uint8_t)0x20U

#define ADT7420_CONTINUOUS_MODE (uint8_t)0x00U
#define ADT7420_ONE_SHOT_MODE (uint8_t)0x40U
#define ADT7420_SPS_MODE (uint8_t)0x60U
#define ADT7420_SHUTDOWN_MODE (uint8_t)0xA0U

#define ADT7420_13_BIT_RES (uint8_t)0x00U
#define ADT7420_16_BIT_RES (uint8_t)0x80U


typedef enum {
	ADT7420_OK,
	ADT7420_INVALID_ADDR,
	ADT7420_INVALID_SETTING,
	ADT7420_I2C_ERROR
} Adt7420_status;

typedef struct {
	uint8_t config;
	int16_t crit_temperature_c;
	int16_t high_temperature_c;
	int16_t low_temperature_c;
	int16_t hysteresis;
} adt7420_settings;

typedef struct {
	I2C_TypeDef* i2c_ch;
	GPIO_TypeDef* int_port;
	uint8_t i2c_addr;
	uint32_t int_pin;
	uint32_t ct_pin;
} adt7420_dev;

Adt7420_status adt7420_write_one_reg(adt7420_dev* dev, uint8_t reg, uint8_t data);
Adt7420_status adt7420_read_one_reg(adt7420_dev* dev, uint8_t reg, uint8_t* data);
Adt7420_status adt7420_write_two_reg(adt7420_dev* dev, uint8_t reg, uint16_t data);
Adt7420_status adt7420_read_two_reg(adt7420_dev* dev, uint8_t reg, uint16_t* data);
Adt7420_status adt7420_init(adt7420_dev* dev, adt7420_settings* params);
Adt7420_status adt7420_on(adt7420_dev* dev);
Adt7420_status adt7420_shutdown(adt7420_dev* dev);
Adt7420_status adt7420_get_status(adt7420_dev* dev, uint8_t* status);
Adt7420_status adt7420_get_config(adt7420_dev* dev, uint8_t* config);
Adt7420_status adt7420_get_temperature(adt7420_dev* dev, float* temp_c);
Adt7420_status adt7420_get_low_temperature_c(adt7420_dev* dev, float* temperature_c);
Adt7420_status adt7420_get_high_temperature_c(adt7420_dev* dev, float* temperature_c);
Adt7420_status adt7420_get_crit_temperature_c(adt7420_dev* dev, float* temperature_c);
Adt7420_status adt7420_get_hysteresis(adt7420_dev* dev, float* hysteresis_c);
Adt7420_status adt7420_set_config(adt7420_dev* dev, uint8_t config);
Adt7420_status adt7420_set_low_temperature_c(adt7420_dev* dev, int16_t temperature_c);
Adt7420_status adt7420_set_high_temperature_c(adt7420_dev* dev, int16_t temperature_c);
Adt7420_status adt7420_set_crit_temperature_c(adt7420_dev* dev, int16_t temperature_c);
Adt7420_status adt7420_set_hysteresis(adt7420_dev* dev, int16_t hysteresis_c);
#endif
