#ifndef BYTE_BUFFER_H
#define BYTE_BUFFER_H

/*
 * Реализация кольцевого буфера
 */

#include <stdint.h>

typedef struct {
	uint8_t 	*buf;
	uint8_t		size;
	uint8_t		max_size;
	uint8_t		start;
	uint8_t		end;
} byte_buf_t;

// Размер буфера должен быть степенью двойки
extern uint8_t init_buf(byte_buf_t *buf, uint8_t *buf_ptr, uint8_t buf_size);

extern uint8_t buf_free_size(byte_buf_t *buf);

extern uint8_t buf_put(byte_buf_t *buf, uint8_t val);
extern uint8_t buf_get(byte_buf_t *buf, uint8_t *val);
extern uint8_t buf_remove_first(byte_buf_t *buf);

extern uint8_t buf_arr_put(byte_buf_t *buf, uint8_t *arr, uint8_t count);
extern uint8_t buf_arr_get(byte_buf_t *buf, uint8_t *arr, uint8_t count);

#endif // BYTE_BUFFER_H
