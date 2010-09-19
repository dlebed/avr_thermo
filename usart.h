#ifndef USART_H
#define USART_H

#include <stdint.h>

#include "config.h"
#include "typedefs.h"

// Размер буфера для отправки данных в байтах
#ifdef MCU_ATMEGA
#define USART_TX_BUF_SIZE 64
#endif

#ifdef MCU_ATTINY
#define USART_TX_BUF_SIZE 4
#endif


// Размер буфера для приёма данных в байтах
#ifdef MCU_ATMEGA
#define USART_RX_BUF_SIZE 64
#endif

#ifdef MCU_ATTINY
#define USART_RX_BUF_SIZE 4
#endif

// Прерывание для отправки данных
// ATMega16
#ifdef MCU_ATMEGA
#define USART_RX_INTERRUPT USART_RXC_vect
#endif
// ATtiny2313
#ifdef MCU_ATTINY
#define USART_RX_INTERRUPT USART_RX_vect
#endif

// Прерывание для приёма данных
// ATMega16
#ifdef MCU_ATMEGA
#define USART_TX_INTERRUPT USART_TXC_vect
#endif
//ATtiny2313
#ifdef MCU_ATTINY
#define USART_TX_INTERRUPT USART_TX_vect
#endif

extern uint8_t init_usart_data(void);

extern uint8_t usart_byte_write(uint8_t byte);

extern uint8_t usart_buf_write(uint8_t *buf, uint8_t size);

extern uint8_t usart_tx_buf_free_size(void);

// Есть или нет что-нибудь в буфере для передачи
extern uint8_t usart_write_more(void);

extern uint8_t usart_tx_interrupt_routine(void);

extern uint8_t usart_rx_interrupt_routine(void);

extern void usart_set_rx_routine(task_ptr_t routine);

// Получить байт из приёмного буфера
extern uint8_t usart_get_byte(uint8_t *byte);

// Получить набор байт из приёмного буфера
extern uint8_t usart_get_buf(uint8_t *buf, uint8_t size);

// Количество байт в приёмном буфере
extern uint8_t usart_rx_size(void);

#endif // USART_H
