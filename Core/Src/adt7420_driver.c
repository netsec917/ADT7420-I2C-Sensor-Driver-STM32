/*
 * adt7420_driver.c
 *
 *  Created on: Aug 23, 2020
 *      Author: Tom
 */

#include "adt7420_driver.h"
#include "stdbool.h"


static uint16_t adt7420_temperature_to_adc_code(uint8_t config, int16_t temp_c);
static float adt7420_adc_code_to_temperature(uint16_t adc_code);
static inline bool adt7420_is_valid_temperature(uint16_t temperature_c, bool hyteresis);
static inline bool adt7420_parse_params(adt7420_settings* params);
static inline void i2c_start_write(I2C_TypeDef* i2c_ch, uint8_t addr, uint8_t n_bytes);
static inline void i2c_start_read(I2C_TypeDef* i2c_ch, uint8_t addr, uint8_t n_bytes);
static inline void i2c_stop(I2C_TypeDef* i2c_ch);
static inline void i2c_write_bytes(I2C_TypeDef* i2c_ch, uint8_t* tx_buf, uint8_t n_bytes);
static inline void i2c_read_bytes(I2C_TypeDef* i2c_ch, uint8_t* rx_buf, uint8_t n_bytes);

static uint16_t adt7420_temperature_to_adc_code(uint8_t config, int16_t temperature_c)
{
	uint16_t adc_code = 0;

	if (config & ADT7420_16_BIT_RES) {
		if (temperature_c < 0) {
			adc_code = (temperature_c * 128) + 65536;
		} else {
			adc_code = temperature_c * 128;
		}
	} else {
		if (temperature_c < 0) {
			// Need to shift left 3 places if we're using 13 bit resolution
			adc_code = (temperature_c * 16) + 8192;
			adc_code = adc_code << 3;
		} else {
			adc_code = temperature_c * 16;
			adc_code = adc_code << 3;
		}
	}
	return adc_code;
}

static float adt7420_adc_code_to_temperature(uint16_t adc_code)
{
	float temperature_c = 0.0;
    if ((adc_code & 0x8000)) {
        if ((adc_code & 0x80)) {
            temperature_c = (((float)adc_code - 65536) / 128);
    	} else {
            adc_code = adc_code >> 3;
            temperature_c = (((float)adc_code - 8192) / 16);
        }
    } else {
        if ((adc_code & 0x80)){
            temperature_c = (float)adc_code / 128;
        } else { 
            adc_code = adc_code >> 3;
            temperature_c = ((float)adc_code / 16);
        }
     }
	return temperature_c;
}


static inline bool adt7420_is_valid_temperature(uint16_t temperature_c, bool hyteresis)
{
	if (!hyteresis) {
		return temperature_c >= ADT7420_MIN_TEMPERATURE_C && temperature_c <= ADT7420_MAX_TEMPERATURE_C;
	} else {
		return temperature_c >= ADT7420_MIN_HYSTERESIS_C && temperature_c <= ADT7420_MAX_HYSTERESIS_C;
	}
}

static inline bool adt7420_parse_params(adt7420_settings* params)
{
	if (!adt7420_is_valid_temperature(params->crit_temperature_c, false)
		|| !adt7420_is_valid_temperature(params->high_temperature_c, false)
		|| !adt7420_is_valid_temperature(params->low_temperature_c, false)) {
		return false;
	}
	if (!adt7420_is_valid_temperature(params->hysteresis, true)) {
		return false;
	}
	return true;
}

static inline void i2c_start_write(I2C_TypeDef* i2c_ch, uint8_t addr, uint8_t n_bytes)
{
	LL_I2C_HandleTransfer(i2c_ch, addr << 1U, LL_I2C_ADDRSLAVE_7BIT, n_bytes, LL_I2C_MODE_SOFTEND, LL_I2C_GENERATE_START_WRITE);
}

static inline void i2c_start_read(I2C_TypeDef* i2c_ch, uint8_t addr, uint8_t n_bytes)
{
	LL_I2C_HandleTransfer(i2c_ch, addr << 1U, LL_I2C_ADDRSLAVE_7BIT, n_bytes, LL_I2C_MODE_SOFTEND, LL_I2C_GENERATE_RESTART_7BIT_READ);
}

static inline void i2c_stop(I2C_TypeDef* i2c_ch)
{
	LL_I2C_GenerateStopCondition(i2c_ch);
	while(!LL_I2C_IsActiveFlag_STOP(i2c_ch));
}

static inline void i2c_write_bytes(I2C_TypeDef* i2c_ch, uint8_t* tx_buf, uint8_t n_bytes)
{
	for (uint8_t i = 0; i < n_bytes; ++i) {
		LL_I2C_TransmitData8(i2c_ch, tx_buf[i]);
		while (!LL_I2C_IsActiveFlag_TXE(i2c_ch));
	}
}

static inline void i2c_read_bytes(I2C_TypeDef* i2c_ch, uint8_t* rx_buf, uint8_t n_bytes)
{
	for (uint8_t i = 0; i < n_bytes; ++i) {
		while (!LL_I2C_IsActiveFlag_RXNE(i2c_ch));
		rx_buf[i] = LL_I2C_ReceiveData8(i2c_ch);
	}
}

Adt7420_status adt7420_write_one_reg(adt7420_dev* dev, uint8_t reg, uint8_t data)
{
	// Make sure the bus is free, before attemping to transmit
	if (LL_I2C_IsActiveFlag_BUSY(dev->i2c_ch)) {
		return ADT7420_I2C_ERROR;
	}
	uint8_t tx_buf[2] = {reg, data};

	i2c_start_write(dev->i2c_ch, dev->i2c_addr, 2);
	i2c_write_bytes(dev->i2c_ch, tx_buf, 2);
	i2c_stop(dev->i2c_ch);

	return ADT7420_OK;
}

Adt7420_status adt7420_read_one_reg(adt7420_dev* dev, uint8_t reg, uint8_t* data)
{
	if (LL_I2C_IsActiveFlag_BUSY(dev->i2c_ch)) {
		return ADT7420_I2C_ERROR;
	}

	i2c_start_write(dev->i2c_ch, dev->i2c_addr, 1);
	i2c_write_bytes(dev->i2c_ch, &reg, 1);
	i2c_start_read(dev->i2c_ch, dev->i2c_addr, 1);
	i2c_read_bytes(dev->i2c_ch, data, 1);
	i2c_stop(dev->i2c_ch);

	return ADT7420_OK;
}

Adt7420_status adt7420_write_two_reg(adt7420_dev* dev, uint8_t reg, uint16_t data)
{
	if (LL_I2C_IsActiveFlag_BUSY(dev->i2c_ch)) {
		return ADT7420_I2C_ERROR;
	}
	// Send the MSB of the uint16_t first after register address
	uint8_t tx_buf[3] = {reg, (data >> 8U), data & 0xFF};

	i2c_start_write(dev->i2c_ch, dev->i2c_addr, 3);
	i2c_write_bytes(dev->i2c_ch, tx_buf, 3);
	i2c_stop(dev->i2c_ch);

	return ADT7420_OK;
}

Adt7420_status adt7420_read_two_reg(adt7420_dev* dev, uint8_t reg, uint16_t* data)
{
	if (LL_I2C_IsActiveFlag_BUSY(dev->i2c_ch)) {
		return ADT7420_I2C_ERROR;
	}

	uint8_t rx_buf[2];
	
	i2c_start_write(dev->i2c_ch, dev->i2c_addr, 1);
	i2c_write_bytes(dev->i2c_ch, &reg, 1);

	i2c_start_read(dev->i2c_ch, dev->i2c_addr, 2);
	i2c_read_bytes(dev->i2c_ch, rx_buf, 2);

	i2c_stop(dev->i2c_ch);

	*data = (rx_buf[0] << 8U) | (rx_buf[1] & 0xFFU);
	return ADT7420_OK;
}


Adt7420_status adt7420_init(adt7420_dev* dev, adt7420_settings* params)
{
	if (!adt7420_parse_params(params)) {
		return ADT7420_INVALID_SETTING;
	}

	uint8_t chip_id;
	if (adt7420_read_one_reg(dev, ADT7420_ID, &chip_id) != ADT7420_OK || chip_id != ADT7420_CHIP_ID) {
		return ADT7420_I2C_ERROR;
	}


	Adt7420_status status;
	status = adt7420_write_one_reg(dev, ADT7420_CONFIG, params->config);

	uint16_t temperature_c;
	temperature_c = adt7420_temperature_to_adc_code(params->config, params->crit_temperature_c);
	status = adt7420_write_two_reg(dev, ADT7420_TEMPERATURE_CRIT_MSB, temperature_c);

	temperature_c = adt7420_temperature_to_adc_code(params->config, params->high_temperature_c);
	status = adt7420_write_two_reg(dev, ADT7420_TEMPERATURE_HIGH_MSB, temperature_c);

	temperature_c = adt7420_temperature_to_adc_code(params->config, params->low_temperature_c);
	status = adt7420_write_two_reg(dev, ADT7420_TEMPERATURE_LOW_MSB, temperature_c);

	temperature_c = adt7420_temperature_to_adc_code(params->config, params->hysteresis);
	status = adt7420_write_two_reg(dev, ADT7420_HYSTERESIS, temperature_c);

	return status;
}

Adt7420_status adt7420_on(adt7420_dev* dev)
{
	uint8_t config;
	if (adt7420_get_config(dev, &config) != ADT7420_OK) {
		return ADT7420_I2C_ERROR;
	}
	// Will set ADT7420 To continuous operation.
	config &= ~ADT7420_SHUTDOWN_MODE;
	if (adt7420_set_config(dev, config) != ADT7420_OK) {
		return ADT7420_I2C_ERROR;
	}
	return ADT7420_OK;
}

Adt7420_status adt7420_shutdown(adt7420_dev* dev)
{
	uint8_t config;
	if (adt7420_get_config(dev, &config) != ADT7420_OK) {
		return ADT7420_I2C_ERROR;
	}

	config |= ADT7420_SHUTDOWN_MODE;
	if (adt7420_set_config(dev, config) != ADT7420_OK) {
		return ADT7420_I2C_ERROR;
	}
	return ADT7420_OK;
}

Adt7420_status adt7420_get_status(adt7420_dev* dev, uint8_t* status)
{
	if (adt7420_read_one_reg(dev, ADT7420_STATUS, status) != ADT7420_OK) {
		return ADT7420_I2C_ERROR;
	}
	return ADT7420_OK;
}

Adt7420_status adt7420_get_config(adt7420_dev* dev, uint8_t* config)
{
	if (adt7420_read_one_reg(dev, ADT7420_CONFIG, config) != ADT7420_OK) {
		return ADT7420_I2C_ERROR;
	}
	return ADT7420_OK;
}

Adt7420_status adt7420_get_temperature(adt7420_dev* dev, float* temperature_c)
{
	uint16_t adc_code;
	if (adt7420_read_two_reg(dev, ADT7420_TEMPERATURE_MSB, &adc_code) != ADT7420_OK) {
		return ADT7420_I2C_ERROR;
	}
	*temperature_c = adt7420_adc_code_to_temperature(adc_code);
	return ADT7420_OK;
}

Adt7420_status adt7420_get_low_temperature_c(adt7420_dev* dev, float* temperature_c)
{
	uint16_t adc_code;
	if (adt7420_read_two_reg(dev, ADT7420_TEMPERATURE_LOW_MSB, &adc_code) != ADT7420_OK) {
		return ADT7420_I2C_ERROR;
	}
	*temperature_c = adt7420_adc_code_to_temperature(adc_code);
	return ADT7420_OK;
}

Adt7420_status adt7420_get_high_temperature_c(adt7420_dev* dev, float* temperature_c)
{	
	uint16_t adc_code;
	if (adt7420_read_two_reg(dev, ADT7420_TEMPERATURE_HIGH_MSB, &adc_code) != ADT7420_OK) {
		return ADT7420_I2C_ERROR;
	}
	*temperature_c = adt7420_adc_code_to_temperature(adc_code);
	return ADT7420_OK;
}

Adt7420_status adt7420_get_crit_temperature_c(adt7420_dev* dev, float* temperature_c)
{
	uint16_t adc_code;
	if (adt7420_read_two_reg(dev, ADT7420_TEMPERATURE_CRIT_MSB, &adc_code) != ADT7420_OK) {
		return ADT7420_I2C_ERROR;
	}
	*temperature_c = adt7420_adc_code_to_temperature(adc_code);
	return ADT7420_OK;
}

Adt7420_status adt7420_get_hysteresis(adt7420_dev* dev, float* hysteresis_c)
{
	uint16_t adc_code;
	if (adt7420_read_two_reg(dev, ADT7420_HYSTERESIS, &adc_code) != ADT7420_OK) {
		return ADT7420_I2C_ERROR;
	}
	*hysteresis_c = adt7420_adc_code_to_temperature(adc_code);
	return ADT7420_OK;
}

Adt7420_status adt7420_set_config(adt7420_dev* dev, uint8_t config)
{
	if (adt7420_write_one_reg(dev, ADT7420_CONFIG, config)) {
		return ADT7420_I2C_ERROR;
	}
	return ADT7420_OK;
}

Adt7420_status adt7420_set_low_temperature_c(adt7420_dev* dev, int16_t temperature_c)
{
	if (!adt7420_is_valid_temperature(temperature_c, false)) {
		return ADT7420_INVALID_SETTING;
	}
	
	uint8_t config;
	if (adt7420_get_config(dev, &config) != ADT7420_OK) {
		return ADT7420_I2C_ERROR;
	}

	uint16_t adc_code = adt7420_temperature_to_adc_code(config, temperature_c);
	if (adt7420_write_two_reg(dev, ADT7420_TEMPERATURE_LOW_MSB, adc_code) != ADT7420_OK) {
		return ADT7420_I2C_ERROR;
	}
	return ADT7420_OK;
}

Adt7420_status adt7420_set_high_temperature_c(adt7420_dev* dev, int16_t temperature_c)
{
	if (!adt7420_is_valid_temperature(temperature_c, false)) {
		return ADT7420_INVALID_SETTING;
	}

	uint8_t config;
	if (adt7420_get_config(dev, &config) != ADT7420_OK) {
		return ADT7420_I2C_ERROR;
	}

	uint16_t adc_code = adt7420_temperature_to_adc_code(config, temperature_c);
	if (adt7420_write_two_reg(dev, ADT7420_TEMPERATURE_HIGH_MSB, adc_code) != ADT7420_OK) {
		return ADT7420_I2C_ERROR;
	}
	return ADT7420_OK;
}

Adt7420_status adt7420_set_crit_temperature_c(adt7420_dev* dev, int16_t temperature_c)
{
	if (!adt7420_is_valid_temperature(temperature_c, false)) {
		return ADT7420_INVALID_SETTING;
	}

	uint8_t config;
	if (adt7420_get_config(dev, &config) != ADT7420_OK) {
		return ADT7420_I2C_ERROR;
	}

	uint16_t adc_code = adt7420_temperature_to_adc_code(config, temperature_c);
	if (adt7420_write_two_reg(dev, ADT7420_TEMPERATURE_CRIT_MSB, adc_code) != ADT7420_OK) {
		return ADT7420_I2C_ERROR;
	}
	return ADT7420_OK;
}

Adt7420_status adt7420_set_hysteresis(adt7420_dev* dev, int16_t hysteresis_c)
{
	if (!adt7420_is_valid_temperature(hysteresis_c, true)) {
		return ADT7420_INVALID_SETTING;
	}

	uint8_t config;
	if (adt7420_get_config(dev, &config) != ADT7420_OK) {
		return ADT7420_I2C_ERROR;
	}

	uint16_t adc_code = adt7420_temperature_to_adc_code(config, hysteresis_c);
	if (adt7420_write_one_reg(dev, ADT7420_HYSTERESIS, adc_code) != ADT7420_OK) {
		return ADT7420_I2C_ERROR;
	}
	return ADT7420_OK;
}
