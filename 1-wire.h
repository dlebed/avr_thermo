#ifndef ONE_WIRE_H_
#define ONE_WIRE_H_

#include <stdint.h>
#include <avr/io.h>

typedef enum {
	OW_E_OK					=	0,
	OW_E_TIMEOUT			=	1,
	OW_E_NO_DEVICES			=	2,
	OW_E_SHORT_CIRCUIT		=	3
} OW_ERROR_CODE;

typedef struct {
  uint8_t* port_in;
  uint8_t* port_out;
  uint8_t* ddr_reg;
  uint8_t pin_mask;
} ow_pin;

/* *** Тайминги  *** */
// Время восстановления линии, минимум 1 мксек
#define OW_RECOVERY_TIME			8

/* Тайминги для команды RESET */
// Время, на которое надо притянуть линию к земле, для обозначения команды RESET
#define OW_RESET_PULLDOWN_TIME		480
// Время ожидания ответа ведомого устройства, после посылки команды RESET
#define OW_RESET_SLAVE_WAIT_TIME	64
// Время, после которого ведомое устройство перестанет удерживать линию притянутой к земле
#define OW_RESET_RESTORE_TIME		256

/* Тайминги для записи/чтения */
// Время удержания линии притянутой к земле для записи нуля
#define OW_0_WRITE_PULLDOWN_TIME	100
// Время удержания линии притянутой к земле для записи единицы
#define OW_1_WRITE_PULLDOWN_TIME	4
// Время ожидания с отпущеной линиец данных при записи единицы
#define OW_1_WRITE_WAIT_TIME		64

// Время удержания линии притянутой к земле для инициации чтения
#define OW_READ_PULLDOWN_TIME		4
// Время ожидания до считывания данных после подачи сигнала на чтение
#define OW_READ_RESULT_WAIT_TIME	8
// Время ожидания завершения такта считывания
#define OW_READ_WAIT_TIME			64

//extern uint8_t ow_set_bus(volatile uint8_t* port_in, volatile uint8_t* port_out, volatile uint8_t* ddr_reg, uint8_t pin_num);

extern uint8_t ow_reset(const ow_pin * pin);

extern uint8_t ow_write_bit(const ow_pin * pin, uint8_t bit);

extern uint8_t ow_read_bit(const ow_pin * pin);

extern uint8_t ow_write_byte(const ow_pin * pin, uint8_t byte);

extern uint8_t ow_read_byte(const ow_pin * pin);

extern uint8_t ow_read_buf(const ow_pin * pin, uint8_t* buf, uint8_t size);

extern uint8_t ow_write_buf(const ow_pin * pin, uint8_t* buf, uint8_t size);


#endif /* ONE_WIRE_H_ */
