#ifndef RTOS_HAL_H
#define RTOS_HAL_H

#include "config.h"

#define STATUS_REG 			SREG
#define INTERRUPT_FLAG		SREG_I
#define DISABLE_INTERRUPT	cli();
#define ENABLE_INTERRUPT	sei();

// Настройки RTOS
// ATmega16
#ifdef MCU_ATMEGA
#define RTOS_ISR  			TIMER0_COMP_vect
#endif

// ATtiny2313
#ifdef MCU_ATTINY
#define RTOS_ISR			TIMER0_COMPA_vect
#endif

extern void run_rtos(void);


#endif // RTOS_HAL_H
