#ifndef DS18X20_H_
#define DS18X20_H_

#include "1-wire.h"

#define DS18X20_CONFIG_REG_MASK		0x1F
#define TEMP_MULTIPLER				625

typedef enum {
	DS18X20_E_OK			=	0,
	DS18X20_E_NO_SENSOR		=	1,
	DS18X20_E_MEASURING		=	2
} DS18X20_ERROR_CODE;

typedef enum {
	DS18X20_ROM_SEARCH			=	0xF0,
	DS18X20_ROM_READ = 0x33,
	DS18X20_ROM_MATCH = 0x55,
	DS18X20_ROM_SKIP = 0xCC,
	DS18X20_ALARM_SEARCH = 0xEC,
	DS18X20_CONVERT_T = 0x44,
	DS18X20_SCRATCHPAD_WRITE = 0x4E,
	DS18X20_SCRATCHPAD_READ = 0xBE,
	DS18X20_SCRATCHPAD_COPY = 0x48,
	DS18X20_POWER_SUPPLY_READ = 0xB4
} DS18X20_CMD;

typedef enum {
	DS18X20_RES_9_BIT		=	0x00,
	DS18X20_RES_10_BIT		=	0x01,
	DS18X20_RES_11_BIT		=	0x02,
	DS18X20_RES_12_BIT		=	0x03
} DS18X20_RESOLUTION;

typedef struct {
  uint8_t t_lsb;       // temperature LSB
  uint8_t t_msb;       // temperature MSB
  int8_t t_h;          // Th register
  int8_t t_l;          // Tl register
  uint8_t conf;      // configuration register
  uint8_t reserved[3]; // reserved
} ds18x20_scratchpad;

typedef struct {
  uint8_t t_h;       // temperature  Th
  uint8_t t_l;       // temperature Tl
  int8_t conf;       // config register
} ds18x20_scratchpad_write;

extern uint8_t ds18x20_init_sensor(const ow_pin * pin);

extern uint8_t ds18x20_send_cmd(const ow_pin * pin, DS18X20_CMD cmd);

extern uint8_t ds18x20_set_resolution(const ow_pin * pin, DS18X20_RESOLUTION resolution);

extern uint8_t ds18x20_start_measurement(const ow_pin * pin);

extern uint8_t ds18x20_read_temp(const ow_pin * pin, int16_t *temp_decicel);

extern uint8_t ds18x20_measurement_status(const ow_pin * pin);



#endif /* DS18X20_H_ */
