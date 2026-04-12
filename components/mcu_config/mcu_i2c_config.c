///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 22.01.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//
#include "mcu_i2c_config.h"
#include <stdlib.h>
#include <string.h>

#define TAG "MCU_I2C"

static mcu_i2c_config_t mcu_i2c_config = {
    .port = CONFIG_I2C_MASTER_PORT_NUM,
    .sda = SDA_GPIO,
    .scl = SCL_GPIO,
    .clk_speed = CONFIG_I2C_MASTER_FREQUENCY,
    .i2c_init_flag = false,
};

SemaphoreHandle_t mcu_i2c_mutex;

esp_err_t mcu_i2c_deinit(void) {
  if (!mcu_i2c_config.i2c_init_flag) {
    return ESP_OK;
  }

  esp_err_t ret = i2c_driver_delete(mcu_i2c_config.port);
  if (ret == ESP_OK) {
    mcu_i2c_config.i2c_init_flag = false;
  } else {
    ESP_LOGE(TAG, "I2C driver delete error: %d", ret);
  }

  return ret;
}

esp_err_t mcu_i2c_init_with_pins(gpio_num_t sda, gpio_num_t scl) {
  if (mcu_i2c_config.i2c_init_flag &&
      mcu_i2c_config.sda == sda &&
      mcu_i2c_config.scl == scl) {
    return ESP_OK;
  }

  if (mcu_i2c_config.i2c_init_flag) {
    esp_err_t deinit_ret = mcu_i2c_deinit();
    if (deinit_ret != ESP_OK) {
      return deinit_ret;
    }
  }

  i2c_config_t conf = {0};
  conf.mode = I2C_MODE_MASTER;
  conf.sda_io_num = sda;
  conf.scl_io_num = scl;
  conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
  conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
  conf.master.clk_speed = mcu_i2c_config.clk_speed;

  esp_err_t ret = i2c_param_config(mcu_i2c_config.port, &conf);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "I2C param config error: %d", ret);
    return ret;
  }

  ret = i2c_driver_install(mcu_i2c_config.port, I2C_MODE_MASTER, 0, 0, 0);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "I2C driver install error: %d", ret);
    return ret;
  }

  mcu_i2c_config.sda = sda;
  mcu_i2c_config.scl = scl;
  mcu_i2c_config.i2c_init_flag = true;

  return ESP_OK;
}

esp_err_t mcu_i2c_init(void) {
  mcu_i2c_mutex = xSemaphoreCreateMutex();
  if (mcu_i2c_mutex == NULL) {
    ESP_LOGE(TAG, "Failed to create I2C mutex");
    return ESP_FAIL;
  }
  return mcu_i2c_init_with_pins(SDA_GPIO, SCL_GPIO);
}

bool _mcu_i2c_write(uint8_t address, uint8_t reg, uint8_t *data, uint8_t len) {
  esp_err_t ret;

  uint8_t *write_buf = malloc(len + 1);
  if (write_buf == NULL) {
    return false;
  }
  write_buf[0] = reg;
  memcpy(write_buf + 1, data, len);
  ret = i2c_master_write_to_device(
      CONFIG_I2C_MASTER_PORT_NUM, address, write_buf, len + 1,
      CONFIG_I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
  free(write_buf);
  return (bool)(ret == ESP_OK);
}

bool _mcu_i2c_read(uint8_t address, uint8_t reg, uint8_t *data, uint8_t len) {
  esp_err_t ret;
  ret = i2c_master_write_read_device(
      CONFIG_I2C_MASTER_PORT_NUM, address, &reg, 1, data, len,
      CONFIG_I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
  return (bool)(ret == ESP_OK);
}