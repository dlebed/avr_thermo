#include "ds18x20.h"


uint8_t ds18x20_init_sensor(const ow_pin * pin) {
	// Сбрасываем датчик
	if (ow_reset(pin) != OW_E_OK)
		return DS18X20_E_NO_SENSOR;

	ds18x20_send_cmd(pin, DS18X20_ROM_SKIP);

	return DS18X20_E_OK;
}

uint8_t ds18x20_send_cmd(const ow_pin * pin, DS18X20_CMD cmd) {
	ow_write_byte(pin, (uint8_t) cmd);
	return DS18X20_E_OK;
}

uint8_t ds18x20_set_resolution(const ow_pin * pin, DS18X20_RESOLUTION resolution) {
	ds18x20_scratchpad_write scratchpad;
	scratchpad.t_h = scratchpad.t_l = 0;
	scratchpad.conf = DS18X20_CONFIG_REG_MASK | ((uint8_t) resolution << 5);

	if (ds18x20_init_sensor(pin) != DS18X20_E_OK)
		return DS18X20_E_NO_SENSOR;

	ds18x20_send_cmd(pin, DS18X20_SCRATCHPAD_WRITE);

	ow_write_buf(pin, (uint8_t*)&scratchpad, sizeof(scratchpad));

	return DS18X20_E_OK;
}

uint8_t ds18x20_start_measurement(const ow_pin * pin) {
	if (ds18x20_init_sensor(pin) != DS18X20_E_OK)
		return DS18X20_E_NO_SENSOR;

	ds18x20_send_cmd(pin, DS18X20_CONVERT_T);

	ow_read_bit(pin);

	return DS18X20_E_OK;
}

uint8_t ds18x20_read_temp(const ow_pin * pin, int16_t *temp_decicel) {
	ds18x20_scratchpad scratchpad;
	int32_t temp = 0;

	if (ds18x20_init_sensor(pin) != DS18X20_E_OK)
		return DS18X20_E_NO_SENSOR;

	ds18x20_send_cmd(pin, DS18X20_SCRATCHPAD_READ);

	ow_read_buf(pin, (uint8_t*)&scratchpad, sizeof(scratchpad));

	temp |= scratchpad.t_lsb;
	temp |= ((uint16_t)scratchpad.t_msb) << 8;

	temp *= TEMP_MULTIPLER;
	temp /= 1000;

	*temp_decicel = (int16_t) temp;

	return DS18X20_E_OK;
}

uint8_t ds18x20_measurement_status(const ow_pin * pin) {
	ds18x20_send_cmd(pin, DS18X20_CONVERT_T);

	if (ow_read_bit(pin) == 0)
		return DS18X20_E_MEASURING;
	else
		return DS18X20_E_OK;
}
