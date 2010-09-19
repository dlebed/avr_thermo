#ifndef ROUTINES_H
#define ROUTINES_H

#include <stdint.h>

typedef enum {
	STATUS_LED_0		= 1 << 0,
	STATUS_MOTOR_SLEEP	= 1 << 1,
	STATE_ON_READ		= 1 << 2,
	STATE_OFF_READ		= 1 << 3
} STATUS_FLAGS;

typedef enum {
	STATE_MOTOR_OFF		=	0,
	STATE_MOTOR_ON		=	1,
	STATE_MOTOR_SLEEP	=	2,
	STATE_MOTOR_WAKEUP	=	3
} MOTOR_STATE;

typedef enum {
	STATE_NULL				=	1 << 0,
	STATE_GET				=	1 << 1,
	STATE_SET				=	1 << 2,
	STATE_SET_TOP			=	1 << 3,
	STATE_SET_BOTTOM		=	1 << 4,
	STATE_SET_MOTOR_WORK	=	1 << 5,
	STATE_SET_MOTOR_SLEEP	=	1 << 6
} USART_READ_STATE;

#define INT16_STR_LEN				8

#define TOP_TEMP						90	// 9 градусов
#define BOTTOM_TEMP						50  // 5 градусов

#define MOTOR_SLEEP_TIME				(60 * 8)  // 8 минут
#define MOTOR_WORK_TO_SLEEP_TIME		(60 * 20) // 20 минуты

extern void routines_init(void);

extern void eeprom_const_read(void);

extern void status_led_blink(void);

extern void sleep_led_on(void);

extern void sleep_led_off(void);

extern void temp_error_led_set(uint8_t is_error);

extern void usart_send_str_word(int16_t num);

extern void set_value_str(uint8_t value_type);

extern void motor_set_state(uint8_t state);

extern void usart_receive_routine(void);

extern void set_relay_state(uint8_t relay, uint8_t state);

extern void temp_measured(void);

extern void temp_check(void);

#endif // ROUTINES_H
