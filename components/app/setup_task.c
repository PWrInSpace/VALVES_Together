///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 06.05.2025 by Szymon Rzewuski
///
///===-----------------------------------------------------------------------------------------===//
///
/// \file
/// This file contains the setup task
///===-----------------------------------------------------------------------------------------===//

#include "setup_task.h"

#include "buzzer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "app_task.h"
#include "board_config.h"
#include "commands.h"
#include "valve_board_config.h"
#include "valves_control.h"

#define SETUP_TASK_STACK_SIZE 8000
#define SETUP_TASK_PRIORITY 9
#define SETUP_TASK_CORE_ID 0

#define TAG "SETUP_TASK"

static TaskHandle_t setup_task_handle = NULL;

static void valve_config_init(void) {
  vTaskDelay(pdMS_TO_TICKS(1000));

#ifdef SERVO_N20_CONFIG
  ESP_LOGI("APP_TASK", "SERVO_N20_CONFIG defined");
  play_sound(SOUND_SINGLE_BEEP);
  vTaskDelay(pdMS_TO_TICKS(1000));
  handle_valve_cmd(N20_VALVE_CLOSE, 0);
#elif defined(SOL_N2_CONFIG)
  ESP_LOGI("APP_TASK", "SOL_N2_CONFIG defined");
  play_sound(SOUND_QUADRUPLE_BEEP);
  vTaskDelay(pdMS_TO_TICKS(1000));
  handle_valve_cmd(N2_SOL_CLOSE, 0);
#elif defined(SOL_ETH_SERVO_N2_CONFIG)
  play_sound(SOUND_TRIPLE_BEEP);
  vTaskDelay(pdMS_TO_TICKS(1000));
  ESP_LOGI("APP_TASK", "SOL_ETH_SERVO_N2_CONFIG defined");
  handle_valve_cmd(ETH_SOL_CLOSE, 0);
  vTaskDelay(pdMS_TO_TICKS(1000));
  handle_valve_cmd(N20_VALVE_CLOSE, 0);
#elif defined(SOL_N20_SERVO_ETH_CONFIG)
  ESP_LOGI("APP_TASK", "SOL_N20_SERVO_ETH_CONFIG defined");
  play_sound(SOUND_DOUBLE_BEEP);
  vTaskDelay(pdMS_TO_TICKS(1000));
  handle_valve_cmd(N20_SOL_CLOSE, 0);
  handle_valve_cmd(ETH_VALVE_CLOSE, 0);
  ESP_LOGI("APP_TASK", "SOL_N2O_N2_CONFIG defined");
#endif

  return;
}

void setup_task(void *arg) {
  if (board_config_init() != ESP_OK) {
    ESP_LOGE(TAG, "Board configuration failed");
    vTaskDelete(NULL);
  }

  valve_config_init();

  // Start the app task
  if (app_task_init() != ESP_OK) {
    ESP_LOGE(TAG, "Failed to initialize app task");
    vTaskDelete(NULL);
  }

  // Delete the setup task
  vTaskDelete(NULL);
}

esp_err_t setup_task_init(void) {

  // Create the setup task
  if (xTaskCreatePinnedToCore(setup_task, "setup_task", SETUP_TASK_STACK_SIZE,
                              NULL, SETUP_TASK_PRIORITY, &setup_task_handle,
                              SETUP_TASK_CORE_ID) == pdPASS) {
    ESP_LOGI(TAG, "Setup task created successfully");
  } else {
    ESP_LOGE(TAG, "Failed to create setup task");
    return ESP_FAIL;
  }

  return ESP_OK;
}

esp_err_t setup_task_deinit(void) {
  if (setup_task_handle != NULL) {
    vTaskDelete(setup_task_handle);
    setup_task_handle = NULL;
  }

  return ESP_OK;
}