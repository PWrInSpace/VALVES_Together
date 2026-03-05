#ifndef ltc4162_h
#define ltc4162_h

#include "mcu_i2c_config.h"

#define LTC4162_I2C_ADDRESS 0x68

esp_err_t ltc4162_read_register(uint8_t reg, uint8_t *data, size_t len);
esp_err_t ltc4162_write_register(uint8_t reg, uint8_t *data, size_t len);
esp_err_t ltc4162_init(void);
esp_err_t ltc4162_debug_monitor(void);

#endif /* ltc4162_h */