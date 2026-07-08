///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 06.05.2025 by Szymon Rzewuski
///
///===-----------------------------------------------------------------------------------------===//
///
/// \file
/// This file contains the main application task, to be implemented by user
///===-----------------------------------------------------------------------------------------===//

#include "app_task.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "BoardData.h"
#include "buzzer.h"
#include "commands.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "valve_board_config.h"
#include "valves_control.h"

#define APP_TASK_STACK_SIZE 8192
#define APP_TASK_PRIORITY 5
#define APP_TASK_CORE_ID 0

static TaskHandle_t app_task_handle = NULL;

esp_err_t app_task_init(void) {

  if (xTaskCreatePinnedToCore(app_task, "app_task", APP_TASK_STACK_SIZE, NULL,
                              APP_TASK_PRIORITY, &app_task_handle,
                              APP_TASK_CORE_ID) == pdPASS) {
    ESP_LOGI("APP_TASK", "App task created successfully");
  } else {
    ESP_LOGE("APP_TASK", "Failed to create app task");
    return ESP_FAIL;
  }

  return ESP_OK;
}

esp_err_t app_task_deinit(void) {
  if (app_task_handle != NULL) {
    vTaskDelete(app_task_handle);
    app_task_handle = NULL;
  }

  return ESP_OK;
}

void app_task(void *arg) {

#ifdef SERVO_N20_CONFIG
  ESP_LOGI("APP_TASK", "SERVO_N20_CONFIG defined");
  vTaskDelay(pdMS_TO_TICKS(1000));
  beep_single();
  vTaskDelay(pdMS_TO_TICKS(1000));
  handle_valve_cmd(N20_VALVE_CLOSE, 0);
#elif defined(SOL_N2_CONFIG)
  ESP_LOGI("APP_TASK", "SOL_N2_CONFIG defined");
  vTaskDelay(pdMS_TO_TICKS(3000));
  beep_quatro();
  vTaskDelay(pdMS_TO_TICKS(1000));
  handle_valve_cmd(N2_SOL_CLOSE, 0);
#elif defined(SOL_ETH_CONFIG)
  ESP_LOGI("APP_TASK", "SOL_ETH_CONFIG defined");
  vTaskDelay(pdMS_TO_TICKS(1000));
  beep_triple();
  vTaskDelay(pdMS_TO_TICKS(1000));
  handle_valve_cmd(ETH_SOL_CLOSE, 0);
#elif defined(SOL_N20_SERVO_ETH_CONFIG)
  ESP_LOGI("APP_TASK", "SOL_N20_SERVO_ETH_CONFIG defined");
  vTaskDelay(pdMS_TO_TICKS(1000));
  beep_double();
  vTaskDelay(pdMS_TO_TICKS(1000));
  handle_valve_cmd(N20_SOL_CLOSE, 0);
  vTaskDelay(pdMS_TO_TICKS(1000));
  handle_valve_cmd(ETH_VALVE_CLOSE, 0);
  ESP_LOGI("APP_TASK", "SOL_N2O_N2_CONFIG defined");
#endif

  while (1) {
    set_boardData_power_time(power_time(), portMAX_DELAY);

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}