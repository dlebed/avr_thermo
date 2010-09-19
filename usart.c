#include "usart.h"

#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include "rtos_hal.h"
#include "rtos.h"
#include "typedefs.h"
#include "byte_buffer.h"

byte_buf_t usart_tx_buf;
uint8_t usart_tx_buf_arr[USART_TX_BUF_SIZE];

byte_buf_t usart_rx_buf;
uint8_t usart_rx_buf_arr[USART_RX_BUF_SIZE];

volatile task_ptr_t rx_routine = 0;

volatile uint8_t usart_status = 0;

inline uint8_t init_usart_data(void) {
	init_buf(&usart_tx_buf, usart_tx_buf_arr, sizeof(usart_tx_buf_arr));
	init_buf(&usart_rx_buf, usart_rx_buf_arr, sizeof(usart_rx_buf_arr));

	return E_OK;
}

uint8_t usart_byte_write(uint8_t byte) {
	uint8_t sreg = STATUS_REG;

	DISABLE_INTERRUPT;

	uint8_t res;

	// Если это первый байт в буфере и USART готов, то запускаем передачу
	if ((usart_tx_buf.size == 0) && (UCSRA & _BV(UDRE))) {
			UDR = byte;
			STATUS_REG = sreg;
			return E_OK;
	}

	// Если USART не готов, то пишем в буффер
	res = buf_put(&usart_tx_buf, byte);

	if (res == E_BUFFER_OVERFLOW) {
		STATUS_REG = sreg;
		return res;
	}

	STATUS_REG = sreg;
	return E_OK;
}

uint8_t usart_buf_write(uint8_t *buf, uint8_t size) {
	uint8_t i, res;

	for (i = 0; i < size; i++) {
		res = usart_byte_write(buf[i]);

		if (res != E_OK)
			return res;
	}

	return E_OK;
}


inline uint8_t usart_tx_buf_free_size() {
	return buf_free_size(&usart_tx_buf);
}

inline uint8_t usart_write_more(void) {
	return usart_tx_buf.size > 0;
}

inline uint8_t usart_tx_interrupt_routine(void) {
	if (usart_write_more()) {
		uint8_t byte;
		buf_get(&usart_tx_buf, &byte);

		UDR = byte;
	}

	return E_OK;
}

inline uint8_t usart_rx_interrupt_routine(void) {
	// Если буфер переполнен, то удаляем самый старый элемент
	if (usart_rx_buf.size == usart_rx_buf.max_size)
		buf_remove_first(&usart_rx_buf);

	buf_put(&usart_rx_buf, UDR);

	if (rx_routine != 0)
		set_task(rx_routine);

	return E_OK;
}

inline void usart_set_rx_routine(task_ptr_t routine) {
	rx_routine = routine;
}

inline uint8_t usart_get_byte(uint8_t *byte) {
	return buf_get(&usart_rx_buf, byte);
}

uint8_t usart_get_buf(uint8_t *buf, uint8_t size) {
	if (size > usart_rx_buf.size)
		return E_BUFFER_UNDERFLOW;

	uint8_t i;

	for (i = 0; i < size; i++)
		buf_get(&usart_rx_buf, buf + i);

	return E_OK;
}

inline uint8_t usart_rx_size(void) {
	return usart_rx_buf.size;
}
