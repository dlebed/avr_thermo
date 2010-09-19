#include <avr/io.h>
#include <avr/interrupt.h>

#include "config.h"
#include "rtos_hal.h"
#include "hal.h"

inline void run_rtos(void) {
	/*
	 * Atmega16 config
	 */
#ifdef MCU_ATMEGA
	TCCR0 = _BV(WGM01) | _BV(CS01) | _BV(CS00);		// Freq = CK/64 - Установить режим CTC и предделитель
													// Автосброс после достижения регистра сравнения
	TCNT0 = 0;										// Установить начальное значение счётчиков
	OCR0  = LO(TIMER_DIVIDER); 						// Установить значение в регистр сравнения

	TIMSK = _BV(OCIE0);
#endif

	/*
	 * ATtiny2313 config
	 */
#ifdef MCU_ATTINY
	TCCR0A = _BV(WGM01);							// Режим - CTC
	TCCR0B =  _BV(CS01) | _BV(CS00);				// Freq = CK/64 - Установить предделитель
													// Автосброс после достижения регистра сравнения
	TCNT0 = 0;										// Установить начальное значение счётчиков
	OCR0A  = LO(TIMER_DIVIDER); 					// Установить значение в регистр сравнения (TOP)

	TIMSK = _BV(OCIE0A);							// Разрешаем прерывание по совпадению таймера
#endif

	ENABLE_INTERRUPT;

}
