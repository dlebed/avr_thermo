#include <avr/io.h>
#include <util/delay.h>

#include "config.h"
#include "hal.h"
#include "routines.h"


inline void init_mcu(void) {

	// Инициализация USART
    UBRRL = LO(BAUDDIVIDER);
    UBRRH = HI(BAUDDIVIDER);
	UCSRA = 0;
	UCSRB = _BV(RXEN) | _BV(TXEN) | _BV(RXCIE) | _BV(TXCIE);

	// ATmega16
#ifdef MCU_ATMEGA
	UCSRC = _BV(URSEL) | _BV(UCSZ0) | _BV(UCSZ1);
#endif

	// ATtiny2313
#ifdef MCU_ATTINY
	UCSRC = _BV(UCSZ0) | _BV(UCSZ1);
#endif

	// Инициализация портов
	// Настраиваем порты на выход
	// Статусный LED
	DDRB |= _BV(PB0) | _BV(PB1);

	//DDRD |= _BV(PD2) | _BV(PD3);
	//DDRD &= ~(_BV(PD4) | _BV(PD5));

	// Начальные значения на ножках
	// Статусный LED
	PORTB |= _BV(PB0);
	PORTB &= ~_BV(PB1);

	// Выходы на реле
	DDRD |= _BV(PD6);
	PORTD &= ~(_BV(PD6));

	// Входы
	//DDRD &= ~(_BV(PD2) | _BV(PD3) | _BV(PD4) | _BV(PD5) | _BV(PD6));

	//PORTD &= ~(_BV(PD2) | _BV(PD3) | _BV(PD4) | _BV(PD5) | _BV(PD6));

	uint8_t i;

	for (i = 0; i < 20; i++) {
		PORTB |= _BV(PB0);
		PORTB &= ~_BV(PB1);
		_delay_ms(30);
		PORTB &= ~(_BV(PB0));
		PORTB |= _BV(PB1);
		_delay_ms(100);
	}

	PORTB &= ~_BV(PB1);
/*
	for (i = 0; i < 2; i++) {
		PORTD |= _BV(PD6);
		_delay_ms(500);
		PORTD &= ~(_BV(PD6));
		_delay_ms(500);
	}
*/
}
