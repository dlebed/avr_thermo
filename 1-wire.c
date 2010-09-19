#include "1-wire.h"

#include <util/delay.h>
#include <util/atomic.h>

/*
uint8_t OW_PIN_MASK;
volatile uint8_t* OW_IN_PORT;
volatile uint8_t* OW_OUT_PORT;
volatile uint8_t* OW_DDR_REG;

#define OW_GET_IN()   ( *OW_IN_PORT & OW_PIN_MASK )
#define OW_OUT_LOW()  ( *OW_OUT_PORT &= (uint8_t) ~OW_PIN_MASK )
#define OW_OUT_HIGH() ( *OW_OUT_PORT |= (uint8_t)  OW_PIN_MASK )
#define OW_DIR_IN()   ( *OW_DDR_REG &= (uint8_t) ~OW_PIN_MASK )
#define OW_DIR_OUT()  ( *OW_DDR_REG |= (uint8_t)  OW_PIN_MASK )
*/

/*
uint8_t ow_set_bus(volatile uint8_t* port_in, volatile uint8_t* port_out, volatile uint8_t* ddr_reg, uint8_t pin_num) {
	OW_DDR_REG = ddr_reg;
	OW_OUT_PORT = port_out;
	OW_IN_PORT = port_in;
	OW_PIN_MASK = (1 << pin_num);
	ow_reset();
	return OW_E_OK;
}
*/

inline uint8_t ow_get_in(const ow_pin * pin) {
	return (*pin->port_in) & pin->pin_mask;
}

inline void ow_out_low(const ow_pin * pin) {
	*pin->port_out &= (uint8_t) ~pin->pin_mask;
}

inline void ow_out_high(const ow_pin * pin) {
	*pin->port_out |= (uint8_t) pin->pin_mask;
}

inline void ow_dir_in(const ow_pin * pin) {
	*pin->ddr_reg &= (uint8_t) ~pin->pin_mask;
}

inline void ow_dir_out(const ow_pin * pin) {
	*pin->ddr_reg |= (uint8_t) pin->pin_mask;
}

uint8_t ow_reset(const ow_pin * pin) {
	uint8_t result = OW_E_OK;

	// Если линия не притянута к питанию, то значит устройство не подключено
	ow_dir_in(pin);
	if (ow_get_in(pin) == 0)
		return OW_E_NO_DEVICES;

	ow_out_low(pin);
	ow_dir_out(pin);            // Притягиваем к земле линию данных на OW_RESET_PULLDOWN_TIME мксек
	_delay_us(OW_RESET_PULLDOWN_TIME);

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		// Ожидаем ведомых устройств
		ow_dir_in(pin);
/*
#if OW_USE_INTERNAL_PULLUP
		OW_OUT_HIGH();
#endif
*/
		_delay_us(OW_RESET_SLAVE_WAIT_TIME);
		// Если линия не подтянута к земле - значит устройства на ней отсутствуют
		if (ow_get_in(pin) != 0)
			result = OW_E_NO_DEVICES;
	}

	_delay_us(OW_RESET_RESTORE_TIME);

	// Если линия не поднялась напряжению питания, то возможо на линии короткое замыкание
	if (ow_get_in(pin) == 0)
		result = OW_E_SHORT_CIRCUIT;

	return result;
}

uint8_t ow_write_bit(const ow_pin * pin, uint8_t bit) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		if (bit) {
			ow_out_low(pin);
			ow_dir_out(pin);
			_delay_us(OW_1_WRITE_PULLDOWN_TIME);
			ow_dir_in(pin);
			_delay_us(OW_1_WRITE_WAIT_TIME + OW_RECOVERY_TIME);
		} else {
			ow_out_low(pin);
			ow_dir_out(pin);
			_delay_us(OW_0_WRITE_PULLDOWN_TIME);
			ow_dir_in(pin);
			_delay_us(OW_RECOVERY_TIME);
		}
	}

	return OW_E_OK;
}

uint8_t ow_read_bit(const ow_pin * pin) {
	uint8_t bit = 0;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		ow_out_low(pin);
		ow_dir_out(pin);
		_delay_us(OW_READ_PULLDOWN_TIME);
		ow_dir_in(pin);
		_delay_us(OW_READ_RESULT_WAIT_TIME);
		bit = (ow_get_in(pin) == 0) ? 0 : 1;
		_delay_us(OW_READ_WAIT_TIME + OW_RECOVERY_TIME);
	}

	return bit;
}

uint8_t ow_write_byte(const ow_pin * pin, uint8_t byte) {
	uint8_t i = 8;

	do {
		ow_write_bit(pin, byte & 1);
		byte >>= 1;
	} while (--i);

	return OW_E_OK;
}

uint8_t ow_read_byte(const ow_pin * pin) {
	uint8_t byte = 0;
	uint8_t i = 8;

	do {
		byte >>= 1;
		byte |= ow_read_bit(pin) << 7;
	} while (--i);

	return byte;
}

uint8_t ow_read_buf(const ow_pin * pin, uint8_t* buf, uint8_t size) {
	uint8_t i;

	for (i = 0; i < size; i++)
		buf[i] = ow_read_byte(pin);

	return OW_E_OK;
}

uint8_t ow_write_buf(const ow_pin * pin, uint8_t* buf, uint8_t size) {
	uint8_t i;

	for (i = 0; i < size; i++)
		ow_write_byte(pin, buf[i]);

	return OW_E_OK;
}
