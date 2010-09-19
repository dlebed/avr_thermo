#ifndef TYPEDEFS_H
#define TYPEDEFS_H

typedef void (*task_ptr_t)(void);

typedef enum {
	E_OK				= 0,
	E_BUFFER_OVERFLOW	= 1,
	E_QUEUE_OVERFLOW	= 2,
	E_QUEUE_EMPTY		= 3,
	E_BUFFER_UNDERFLOW  = 4
} ERROR_CODE;

#endif // TYPEDEFS_H
