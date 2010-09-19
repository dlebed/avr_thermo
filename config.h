#ifndef CONFIG_H
#define CONFIG_H

// Частота тактового генератора- задаётся в makefile
//#define F_CPU 8000000UL

// Для микроконтроллеров ATMega8/16
#if (defined(__AVR_ATmega8__) || defined(__AVR_ATmega16__) || defined(__AVR_ATmega32__) || defined(__AVR_ATmega48__) || \
     defined(__AVR_ATmega16A__) || defined(__AVR_ATmega48A__))
#define MCU_ATMEGA
#endif

// Для микроконтроллера Attiny2313
#if (defined(__AVR_ATtiny2313__) || defined(__AVR_ATtiny4313__) || \
     defined(__AVR_ATtiny2313A__))
#define MCU_ATTINY
#endif

// Размер очереди заданий, должно быть степенью двойки
#ifdef MCU_ATMEGA
#define	TASK_QUEUE_SIZE				32
#endif

#ifdef MCU_ATTINY
#define	TASK_QUEUE_SIZE				8
#endif

// Размер очереди таймера, должен быть степенью двойки
#ifdef MCU_ATMEGA
#define MAIN_TIMER_QUEUE_SIZE		16
#endif

#ifdef MCU_ATTINY
#define MAIN_TIMER_QUEUE_SIZE		4
#endif



/*
 * Усыплять процессор, когда нет задач для выполнения.
 * После усыпления процессор будет реагировать только на прерывания.
 * Время пробуждения - 4 - 6 тиков тактового генератора.
 */
#define TASK_MANAGER_SLEEP_MODE

#endif // CONFIG_H
