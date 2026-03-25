#ifndef PWRINSPACE_MCU_GPIO_CONFIG_H_
#define PWRINSPACE_MCU_GPIO_CONFIG_H_

#include <stdbool.h>
#include <stdint.h>

#include "driver/gpio.h"
#include "mcu_pinout.h"
#include "rom/gpio.h"
#include "soc/gpio_struct.h"

typedef enum {
  SOFT_ARM_GPIO = _SOFT_ARM_GPIO,
  DUMP_VALVE_GPIO = _DUMP_VALVE_GPIO,
} mcu_gpio_cfg_t;

typedef enum {
  SOFT_ARM_GPIO_INDEX = 0,
  DUMP_VALVE_GPIO_INDEX,
  MAX_GPIO_INDEX,
} mcu_gpio_index_cfg_t;

typedef struct {
  uint8_t pins[MAX_GPIO_INDEX];
  uint8_t num_pins;
  gpio_config_t configs[MAX_GPIO_INDEX];
} mcu_gpio_config_t;

esp_err_t mcu_gpio_init(void);

bool _mcu_gpio_set_level(uint8_t gpio, uint8_t level);

bool _mcu_gpio_get_level(uint8_t gpio, uint8_t *level);

bool _relay_gpio_set_level(uint8_t gpio, bool level);

#endif /* PWRINSPACE_MCU_GPIO_CONFIG_H_ */