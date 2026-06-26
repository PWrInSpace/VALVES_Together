#ifndef ltc4162_h
#define ltc4162_h

#include "mcu_i2c_config.h"

#define LTC4162_I2C_ADDRESS 0x68

typedef struct {
  float vbat;
  float vin;
  float ibat;
  float iin;
  float die_temp;
  float vout;
  bool charger_status;
  bool charger_state;
} ltc4162_charger_data_t;

esp_err_t ltc4162_read_register(uint8_t reg, uint8_t *data, size_t len);
esp_err_t ltc4162_write_register(uint8_t reg, uint8_t *data, size_t len);
esp_err_t ltc4162_init(void);
esp_err_t read_charger_data(ltc4162_charger_data_t *charger_data);
esp_err_t ltc4162_debug_monitor(void);

#endif /* ltc4162_h */