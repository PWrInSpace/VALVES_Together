///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 06.05.2025 by Szymon Rzewuski
///
///===-----------------------------------------------------------------------------------------===//
///
/// \file
/// This file contains implementation of the system configuration including mcu
/// config, can api config and hardware config to be implemented by user
///===-----------------------------------------------------------------------------------------===//

#include "board_config.h"

#include "driver/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "BoardData.h"
#include "buzzer.h"
#include "console_config.h"
#include "igniter_driver.h"
#include "ltc4162.h"
#include "mcu_adc_config.h"
#include "mcu_gpio_config.h"
#include "mcu_i2c_config.h"
#include "mcu_spi_config.h"
#include "now.h"
#include "sd_task.h"
#include "servo_config.h"
#include "solenoid_config.h"
#include "timers_config.h"

#define TAG "BOARD_CONFIG"

void _led_delay(uint32_t _ms) { vTaskDelay(_ms / portTICK_PERIOD_MS); }

board_config_t config = {.board_name = CONFIG_NAME};

esp_err_t board_config_init(void) {

  esp_err_t err;

  // Initialize board data structure and semaphore
  err = board_data_init();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Board data initialization failed");
    return err;
  }

  err = mcu_gpio_init();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "GPIO initialization failed");
    return err;
  }

  err = mcu_spi_init();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "SPI initialization failed");
    return err;
  }

  if (!timers_init()) {
    ESP_LOGE(TAG, "Failed to initialize timers");
    return ESP_FAIL;
  }

  if (nowInit()) {
    nowAddPeer(adressObc, 1);
    uint8_t mac[6];
    esp_wifi_get_mac(ESP_IF_WIFI_STA, mac);
    ESP_LOGI("MAC address", "MAC address: %02x:%02x:%02x:%02x:%02x:%02x",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  } else {
    ESP_LOGE(TAG, "ESP-NOW initialization failed");
    return ESP_FAIL;
  }

  err = valves_init();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Valves initialization failed");
    return err;
  }

  err = init_multiple_servos();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Servo configuration failed");
    vTaskDelete(NULL);
  }

  err = mcu_i2c_init();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "I2C initialization failed");
    vTaskDelete(NULL);
  }

  err = console_config_init();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Console configuration failed");
    vTaskDelete(NULL);
  }

  err = ltc4162_init();
  if (err != ESP_OK) {
    ESP_LOGW(TAG, "LTC4162 initialization failed");
    ESP_LOGW(TAG, "Connect vbat or vin");
  }

  err = buzzer_init();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Buzzer initialization failed");
    vTaskDelete(NULL);
  }

  if (mcu_adc_init() != ESP_OK) {
    ESP_LOGE(TAG, "ADC initialization failed");
    return ESP_FAIL;
  }

  err = igniter_init();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Igniter initialization failed");
    vTaskDelete(NULL);
  }

  // if (sd_task_init() != ESP_OK) {
  //   ESP_LOGE(TAG, "SD card task initialization failed");
  //   return ESP_FAIL;
  // }

  createNowSendTask();
  // pressure_task_init();
  // voltage_task_init();
  // temperature_task_init();

  return ESP_OK;

  //*********** ADD HARDWARE CONFIGURATION HERE ***********//
}