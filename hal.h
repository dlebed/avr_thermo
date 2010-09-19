#ifndef HAL_H
#define HAL_H

#include "config.h"

// Настройка системного таймера
#define TIMER_PRESCALER	  		64
#define	TIMER_DIVIDER	  		(F_CPU/TIMER_PRESCALER/1000)		// 1 mS

// Настройка USART
#define BAUDRATE 19200UL
#define BAUDDIVIDER ((F_CPU + BAUDRATE * 8) / (BAUDRATE * 16) - 1)
#define HI(x) ((x) >> 8)
#define LO(x) ((x) & 0xFF)

extern void init_mcu(void);



#endif // HAL_H
