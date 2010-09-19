#ifndef RTOS_H
#define RTOS_H

#include <stdint.h>

#include "config.h"
#include "typedefs.h"

// Маски для работы с кольцевым буфером
#define TASK_QUEUE_SIZE_MASK		(TASK_QUEUE_SIZE - 1)
#define MAIN_TIMER_QUEUE_SIZE_MASK	(MAIN_TIMER_QUEUE_SIZE - 1)

typedef struct {
	task_ptr_t 	task_ptr;
	uint16_t   	task_time;
	uint16_t   	task_time_elapsed;
	uint8_t		flags;			// Флаги (см. TIMER_TASK_FLAGS)
} timer_task_t;

typedef enum {
	FLAG_PERSISTENT		= 1 << 0	// Задача будет вызываться постоянно, а не один раз
} TIMER_TASK_FLAGS;

typedef struct {
	task_ptr_t 	queue[TASK_QUEUE_SIZE];
	uint8_t		size;
	uint8_t		start;
	uint8_t		end;
} task_queue_t;

extern void init_rtos(void);

extern uint8_t set_task(task_ptr_t task);

extern uint8_t task_manager(void);

extern uint8_t set_timer_task(task_ptr_t task, int16_t time_msec, uint8_t flags);

extern uint8_t remove_timer_task(task_ptr_t task);

extern uint8_t timer_manager(void);

#endif // RTOS_H
