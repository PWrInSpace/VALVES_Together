#ifndef THERMOCOUPLE_CONFIG_H
#define THERMOCOUPLE_CONFIG_H

#include "esp_err.h"
#include "max31856.h"

#define THERMOCOUPLE_COUNT 3

#define TC_SCK_GPIO 4
#define TC_MISO_GPIO 5
#define TC_MOSI_GPIO 6
#define TC0_CS_GPIO 16
#define TC1_CS_GPIO 15
#define TC2_CS_GPIO 7

#define TC_SPI_HOST SPI3_HOST
#define THERMOCOUPLE_TYPE MAX31856_TCTYPE_K

extern max31856_cfg thermocouple_devices[THERMOCOUPLE_COUNT];

esp_err_t thermocouple_config_init(void);

#endif /* THERMOCOUPLE_CONFIG_H */
