///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 15.05.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//

#include "mcu_spi_config.h"

#include "esp_log.h"
#include "esp_rom_sys.h"
#define TAG "MCU_SPI"

static mcu_spi_config_t spi_config = MCU_SPI_DEFAULT_CONFIG();
SemaphoreHandle_t mutex_spi;

esp_err_t mcu_spi_init(void) {
  esp_err_t ret = ESP_OK;
  if (spi_config.spi_init_flag) {
    return ESP_OK;
  }

  ret = spi_bus_initialize(spi_config.host_id, &spi_config.bus_config,
                           SDSPI_DEFAULT_DMA);
  ESP_ERROR_CHECK(ret);

  ESP_ERROR_CHECK(ret);
  mutex_spi = xSemaphoreCreateMutex();
  spi_config.spi_init_flag = true;
  return ret;
}

esp_err_t mcu_spi_deinit(void) {
  esp_err_t ret = ESP_OK;
  if (!spi_config.spi_init_flag) {
    return ESP_OK;
  }
  ret = spi_bus_remove_device(spi_config.spi_ads1256_handle);
  ESP_ERROR_CHECK(ret);
  ret = spi_bus_free(spi_config.host_id);
  ESP_ERROR_CHECK(ret);
  spi_config.spi_init_flag = false;
  return ret;
}
