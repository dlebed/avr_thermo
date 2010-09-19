#include <util/atomic.h>
#include <avr/interrupt.h>
#include <avr/sfr_defs.h>
#include <avr/wdt.h>
#include <stdlib.h>

#include "rtos.h"
#include "rtos_hal.h"
#include "typedefs.h"
#include "config.h"

// Очередь задач
volatile static task_queue_t TaskQueue;

// Очередь таймера
volatile static timer_task_t TimerQueue[MAIN_TIMER_QUEUE_SIZE];

inline void init_rtos() {
	uint8_t i;

	TaskQueue.size = 0;
	TaskQueue.end = 0;
	TaskQueue.start = 0;

	for (i = 0; i < TASK_QUEUE_SIZE; i++)
		TaskQueue.queue[i] = NULL;

	for (i = 0; i < MAIN_TIMER_QUEUE_SIZE; i++) {
		TimerQueue[i].task_ptr = NULL;
	}
}

uint8_t set_task(task_ptr_t task) {
	uint8_t sreg = STATUS_REG;

	DISABLE_INTERRUPT;

	if (TaskQueue.size == TASK_QUEUE_SIZE) {
		STATUS_REG = sreg;
		return E_QUEUE_OVERFLOW;
	}

	TaskQueue.queue[TaskQueue.end++] = task;
	TaskQueue.end &= TASK_QUEUE_SIZE_MASK;
	TaskQueue.size++;

	STATUS_REG = sreg;
	return E_OK;
}

// Запускает первую задачу из очереди, если она не пуста
inline uint8_t task_manager() {
	DISABLE_INTERRUPT;

	if (TaskQueue.size == 0) {
		ENABLE_INTERRUPT;
		return E_QUEUE_EMPTY;
	}

	if (TaskQueue.queue[TaskQueue.start] != NULL)
		(TaskQueue.queue[TaskQueue.start++])();

	TaskQueue.start &= TASK_QUEUE_SIZE_MASK;
	TaskQueue.size--;

	ENABLE_INTERRUPT;
	return E_OK;
}

uint8_t set_timer_task(task_ptr_t task, int16_t time_msec, uint8_t flags) {
	uint8_t sreg = STATUS_REG;
	uint8_t index;

	DISABLE_INTERRUPT;

	// Ищем свободный слот для таймера
	for (index = 0; index < MAIN_TIMER_QUEUE_SIZE; index++) {
		if (TimerQueue[index].task_ptr == NULL)
			break;
	}

	// Если слотов нет
	if ((index == MAIN_TIMER_QUEUE_SIZE) && (TimerQueue[index].task_ptr != NULL)) {
		STATUS_REG = sreg;
		return E_QUEUE_OVERFLOW;
	}

	TimerQueue[index].task_ptr = task;
	TimerQueue[index].task_time = time_msec;
	TimerQueue[index].task_time_elapsed = time_msec;
	TimerQueue[index].flags = flags;

	STATUS_REG = sreg;
	return E_OK;
}

uint8_t remove_timer_task(task_ptr_t task) {
	uint8_t sreg = STATUS_REG;
	uint8_t index;

	DISABLE_INTERRUPT;

	for (index = 0; index < MAIN_TIMER_QUEUE_SIZE; index++) {
		if (TimerQueue[index].task_ptr == task)
			TimerQueue[index].task_ptr = NULL;
	}

	STATUS_REG = sreg;
	return E_OK;
}

// Выполняется в прерывании таймера
inline uint8_t timer_manager(void) {
	uint8_t index;

	for (index = 0; index < MAIN_TIMER_QUEUE_SIZE; index++) {
		if (TimerQueue[index].task_ptr == NULL)
			continue;

		if (TimerQueue[index].task_time_elapsed > 0) {
			TimerQueue[index].task_time_elapsed--;
		} else {
			set_task(TimerQueue[index].task_ptr);

			if (TimerQueue[index].flags & FLAG_PERSISTENT)
				TimerQueue[index].task_time_elapsed = TimerQueue[index].task_time;
			else
				TimerQueue[index].task_ptr = NULL;
		}
	}

	return E_OK;
}
