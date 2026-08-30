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

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "BoardData.h"
#include "RGB_led_driver.h"
#include "auto_vent_task.h"
#include "buzzer.h"
#include "buzzer_task.h"
#include "console_config.h"
#include "flash.h"
#include "igniter_driver.h"
#include "igniter_task.h"
#include "ltc4162.h"
#include "mcu_adc_config.h"
#include "mcu_gpio_config.h"
#include "mcu_i2c_config.h"
#include "measure_task.h"
#include "now.h"
#include "pressure_driver.h"
#include "sd_task.h"
#include "servo_config.h"
#include "solenoid_config.h"
#include "thermocouple_task.h"
#include "timers_config.h"
#include "valve_board_config.h"

#define TAG "BOARD_CONFIG"

static void _led_delay(uint32_t _ms) { vTaskDelay(_ms / portTICK_PERIOD_MS); }

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

  if (!run_buzzer_task()) {
    ESP_LOGE(TAG, "Buzzer task initialization failed");
    return ESP_FAIL;
  }

#ifdef SERVO_N20_CONFIG
  err = thermocouple_config_init();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Thermocouple initialization failed");
    return err;
  }
#endif

  err = timers_init();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Failed to initialize timers");
    return ESP_FAIL;
  }

  err = flash_init();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "NVS initialization failed");
    return err;
  }

  if (nowInit() == ESP_OK) {
    nowAddPeer(addressObc, 1);
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

  err = ltc4162_init(&LTC4162_DEFAULT_CONFIG(GPIO_NUM_NC));
  if (err != ESP_OK) {
    ESP_LOGW(TAG, "LTC4162 initialization failed");
    ESP_LOGW(TAG, "Connect vbat or vin");
  }

  err = buzzer_init();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Buzzer initialization failed");
    vTaskDelete(NULL);
  }

  err = mcu_adc_init();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "ADC initialization failed");
    return ESP_FAIL;
  }

  err = igniter_init();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Igniter initialization failed");
    vTaskDelete(NULL);
  }

  pressure_driver_status_t ret_press;
  ret_press = pressure_driver_init(&(pressure_driver_config));
  if (ret_press != PRESSURE_DRIVER_OK) {
    ESP_LOGE(TAG, "Failed to initialize pressure driver");
    return ESP_FAIL;
  } else {
    ESP_LOGI(TAG, "Pressure driver 1 initialized");
    apply_pressure_calibration();
  }

  if (sd_task_init() != ESP_OK) {
    ESP_LOGE(TAG, "SD card task initialization failed");
    play_buzzer_sound(SOUND_INIT_ERROR);
    // return ESP_FAIL;
  }

  createNowSendTask();
  run_measure_task();

#ifdef SOL_N20_SERVO_ETH_CONFIG
  run_auto_vent_task();
#endif
  return ESP_OK;

  //*********** ADD HARDWARE CONFIGURATION HERE ***********//
}