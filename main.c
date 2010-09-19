#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/sfr_defs.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

// RTOS
#include "hal.h"
#include "rtos.h"
#include "rtos_hal.h"

#include "usart.h"

// Функции, специфичные для проекта
#include "routines.h"

// Прерывание таймера для RTOS
ISR (RTOS_ISR) {
	timer_manager();
}

ISR (USART_TX_INTERRUPT) {
	usart_tx_interrupt_routine();
}

ISR (USART_RX_INTERRUPT) {
	usart_rx_interrupt_routine();
}

int main(void) {
	init_mcu();
	init_rtos();
	init_usart_data();
	run_rtos();

	wdt_enable(WDTO_120MS);

	routines_init();

	while (1) {
#ifdef TASK_MANAGER_SLEEP_MODE
		if (task_manager() == E_QUEUE_EMPTY) {
			sleep_mode();
		}
#else
		task_manager();
#endif

		wdt_reset();
	}

	return 0;
}
