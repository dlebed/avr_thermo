#include "routines.h"

#include <avr/io.h>
#include <avr/eeprom.h>
#include <stdlib.h>
#include <string.h>

#include "rtos.h"
#include "usart.h"
#include "1-wire.h"
#include "ds18x20.h"

volatile uint8_t status = 0;
volatile uint8_t motor_state	=	STATE_MOTOR_OFF;
volatile uint16_t motor_work_to_sleep_timer	=	0;
volatile uint16_t usart_read_state	=	STATE_NULL;

const ow_pin temp_sensor_pin = { (uint8_t*)&PINA, (uint8_t*)&PORTA, (uint8_t*)&DDRA, 0 };

int16_t last_measured_temp	=	0xFFFF;

char set_value_buf[INT16_STR_LEN];
uint8_t set_value_buf_pos	=	0;

int16_t top_temp								=	TOP_TEMP;
int16_t EEMEM top_temp_eeprom					=	TOP_TEMP;
int16_t bottom_temp								=	BOTTOM_TEMP;
int16_t EEMEM bottom_temp_eeprom				=	BOTTOM_TEMP;


uint16_t motor_sleep_time						=	MOTOR_SLEEP_TIME;
uint16_t EEMEM motor_sleep_time_eeprom			=	MOTOR_SLEEP_TIME;
uint16_t motor_work_to_sleep_time				=	MOTOR_WORK_TO_SLEEP_TIME;
uint16_t EEMEM motor_work_to_sleep_time_eeprom	=	MOTOR_WORK_TO_SLEEP_TIME;

inline void routines_init(void) {
	usart_set_rx_routine(usart_receive_routine);
	//ow_set_bus(&PINA, &PORTA, &DDRA, 0);
	ds18x20_set_resolution(&temp_sensor_pin, DS18X20_RES_10_BIT);

	eeprom_const_read();

	set_timer_task(status_led_blink, 500, FLAG_PERSISTENT);
	set_timer_task(temp_check, 1000, FLAG_PERSISTENT);
}

void eeprom_const_read(void) {
	top_temp = eeprom_read_word((uint16_t *) &top_temp_eeprom);
	bottom_temp = eeprom_read_word((uint16_t *) &bottom_temp_eeprom);

	motor_sleep_time = eeprom_read_word(&motor_sleep_time_eeprom);
	motor_work_to_sleep_time = eeprom_read_word(&motor_work_to_sleep_time_eeprom);
}

void status_led_blink(void) {
	if (status & STATUS_LED_0) {
		PORTB |= _BV(PB0);
		status &= ~ (STATUS_LED_0);
	} else {
		PORTB &= ~_BV(PB0);
		status |= STATUS_LED_0;
	}
}

void sleep_led_on(void) {
	if (status & STATUS_MOTOR_SLEEP) {
		PORTB |= _BV(PB1);
		set_timer_task(sleep_led_off, 100, 0);
	}
}

void sleep_led_off(void) {
	if (status & STATUS_MOTOR_SLEEP) {
		PORTB &= ~_BV(PB1);
	}
}

void temp_error_led_set(uint8_t is_error) {
	if (is_error)
		PORTB |= _BV(PB1);
	else
		PORTB &= ~_BV(PB1);
}

void usart_send_str_word(int16_t num) {
	char temp_str[INT16_STR_LEN];

	uint8_t len = 0;

	if (itoa(num, temp_str, 10) == NULL)
		return;

	len = strnlen(temp_str, INT16_STR_LEN);

	usart_buf_write((uint8_t *)temp_str, len);
}

void set_value_str(uint8_t value_type) {
	int16_t value;

	value = atoi(set_value_buf);

	switch (value_type) {
	case STATE_SET_TOP:
		top_temp = value;
		eeprom_write_word((uint16_t *) &top_temp_eeprom, top_temp);
		break;
	case STATE_SET_BOTTOM:
		bottom_temp = value;
		eeprom_write_word((uint16_t *) &bottom_temp_eeprom, bottom_temp);
		break;
	case STATE_SET_MOTOR_SLEEP:
		motor_sleep_time = value;
		eeprom_write_word(&motor_sleep_time_eeprom, motor_sleep_time);
		break;
	case STATE_SET_MOTOR_WORK:
		motor_work_to_sleep_time = value;
		eeprom_write_word(&motor_work_to_sleep_time_eeprom, motor_work_to_sleep_time);
		break;
	default:
		break;
	}
}

void motor_set_state(uint8_t state) {
	switch (state) {
	case STATE_MOTOR_WAKEUP:
		if (motor_state == STATE_MOTOR_SLEEP) {
			PORTD |= _BV(PD6);
			motor_state = STATE_MOTOR_ON;
			motor_work_to_sleep_timer = 0;
			temp_error_led_set(0);
			status &= ~ (STATUS_MOTOR_SLEEP);
			remove_timer_task(sleep_led_on);
		}

		break;

	case STATE_MOTOR_ON:
		switch (motor_state) {
		case STATE_MOTOR_OFF:
			PORTD |= _BV(PD6);
			motor_state = STATE_MOTOR_ON;
			motor_work_to_sleep_timer = 0;
			break;
		case STATE_MOTOR_SLEEP:
		case STATE_MOTOR_ON:
		default:
			break;
		}

		break;

	case STATE_MOTOR_SLEEP:
		switch (motor_state) {
		case STATE_MOTOR_ON:
			PORTD &= ~_BV(PD6);
			motor_state = STATE_MOTOR_SLEEP;
			motor_work_to_sleep_timer = 0;
			status |= STATUS_MOTOR_SLEEP;
			set_timer_task(sleep_led_on, 2000, FLAG_PERSISTENT);
			break;
		case STATE_MOTOR_SLEEP:
		case STATE_MOTOR_OFF:
		default:
			break;
		}

		break;

	case STATE_MOTOR_OFF:
		switch (motor_state) {
		case STATE_MOTOR_SLEEP:
		case STATE_MOTOR_ON:
			PORTD &= ~_BV(PD6);
			motor_state = STATE_MOTOR_OFF;
			motor_work_to_sleep_timer = 0;
			break;
		case STATE_MOTOR_OFF:
			PORTD &= ~_BV(PD6);
		default:
			break;
		}

		break;
	}
}

void temp_measured(void) {
	int16_t temp;

	if (ds18x20_read_temp(&temp_sensor_pin, &temp) != DS18X20_E_OK) {
		temp_error_led_set(1);
		motor_set_state(STATE_MOTOR_OFF);
		return;
	}

	temp_error_led_set(0);
	last_measured_temp = temp;

	if (temp > top_temp) {
		motor_set_state(STATE_MOTOR_ON);
	} else if (temp < bottom_temp) {
		motor_set_state(STATE_MOTOR_OFF);
	}
}

void usart_receive_routine(void) {
	uint8_t byte = 0;

	while (usart_get_byte(&byte) != E_BUFFER_UNDERFLOW) {
		// Если от нас хотят получить какое-то значение
		if (usart_read_state == STATE_GET) {
			usart_read_state = STATE_NULL;

			switch (byte) {
			case 't':
				usart_send_str_word(top_temp);
				return;
			case 'b':
				usart_send_str_word(bottom_temp);
				return;
			case 'c':
				usart_send_str_word(last_measured_temp);
				return;
			case 'w':
				usart_send_str_word(motor_work_to_sleep_time);
				return;
			case 's':
				usart_send_str_word(motor_sleep_time);
				return;
			case 'm':
				usart_send_str_word(motor_work_to_sleep_timer);
				return;
			default:
				break;
			}
		}

		if (usart_read_state == STATE_SET) {
			switch (byte) {
			case 't':
				usart_read_state = STATE_SET_TOP;
				return;
			case 'b':
				usart_read_state = STATE_SET_BOTTOM;
				return;
			case 'w':
				usart_read_state = STATE_SET_MOTOR_WORK;
				return;
			case 's':
				usart_read_state = STATE_SET_MOTOR_SLEEP;
				return;
			default:
				usart_read_state = STATE_NULL;
				break;
			}
		}

		if (usart_read_state & (STATE_SET_TOP | STATE_SET_BOTTOM | STATE_SET_MOTOR_WORK | STATE_SET_MOTOR_SLEEP)) {
			if ((byte == '\n') || (set_value_buf_pos == sizeof(set_value_buf))) {
				set_value_str(usart_read_state);
				set_value_buf_pos = 0;
				usart_read_state = STATE_NULL;
				memset(set_value_buf, 0, sizeof(set_value_buf));
			} else {
				set_value_buf[set_value_buf_pos++] = byte;
			}
			return;
		}

		if (byte == 's')
			usart_read_state = STATE_SET;

		if (byte == 'g')
			usart_read_state = STATE_GET;

		if (byte == 'p')
			usart_byte_write(byte);


    }
}

void set_relay_state(uint8_t relay, uint8_t state) {
	if (state)
		PORTB |= _BV(relay);
	else
		PORTB &= ~_BV(relay);
}

void temp_check(void) {
	if (ds18x20_start_measurement(&temp_sensor_pin) != DS18X20_E_OK) {
		temp_error_led_set(1);
		motor_set_state(STATE_MOTOR_OFF);
		return;
	} else {
		temp_error_led_set(0);
	}

	switch (motor_state) {
	case STATE_MOTOR_ON:
		motor_work_to_sleep_timer++;
		// Если мотор долго работал то дадим ему отдохнуть
		if (motor_work_to_sleep_timer >= motor_work_to_sleep_time)
			motor_set_state(STATE_MOTOR_SLEEP);

		break;
	case STATE_MOTOR_SLEEP:
		motor_work_to_sleep_timer++;
		// Если мотор долго отдыхал, то пора поработать
		if (motor_work_to_sleep_timer >= motor_sleep_time)
			motor_set_state(STATE_MOTOR_WAKEUP);

		break;
	default:
		break;
	}

	set_timer_task(temp_measured, 256, 0);
}
