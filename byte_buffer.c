#include "byte_buffer.h"
#include "typedefs.h"

uint8_t init_buf(byte_buf_t *buf, uint8_t *buf_ptr, uint8_t buf_size) {
	buf->buf = buf_ptr;
	buf->max_size = buf_size;
	buf->size = 0;
	buf->start = buf->end = 0;

	return E_OK;
}

inline uint8_t buf_free_size(byte_buf_t *buf) {
	return buf->max_size - buf->size;
}

uint8_t buf_put(byte_buf_t *buf, uint8_t val) {
	if (buf->size == buf->max_size)
		return E_BUFFER_OVERFLOW;

	buf->buf[buf->end++] = val;
	buf->end &= (buf->max_size - 1);
	buf->size++;

	return E_OK;
}


uint8_t buf_get(byte_buf_t *buf, uint8_t *val) {
	if (buf->size == 0)
		return E_BUFFER_UNDERFLOW;

	*val = buf->buf[buf->start++];
	buf->start &= (buf->max_size - 1);
	buf->size--;

	return E_OK;
}

uint8_t buf_remove_first(byte_buf_t *buf) {
	if (buf->size == 0)
		return E_BUFFER_UNDERFLOW;

	buf->start++;
	buf->start &= (buf->max_size - 1);
	buf->size--;

	return E_OK;
}

uint8_t buf_arr_put(byte_buf_t *buf, uint8_t *arr, uint8_t count) {
	uint8_t i, res;

	for (i = 0; i < count; i++) {
		res = buf_put(buf, arr[i]);

		if (res != E_OK)
			return res;
	}

	return E_OK;
}

uint8_t buf_arr_get(byte_buf_t *buf, uint8_t *arr, uint8_t count) {
	uint8_t i, res;

	for (i = 0; i < count; i++) {
		res = buf_get(buf, &(arr[i]));

		if (res != E_OK)
			return res;
	}

	return E_OK;
}
