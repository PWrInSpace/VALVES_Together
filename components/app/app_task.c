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

#include "esp_log.h"
#include "pressure_sensor.h"
#include "BoardData.h"
#include "voltage_measure.h"
#include "valves_control.h"
#include "commands.h"
#include "esp_timer.h"


#define APP_TASK_STACK_SIZE 8192
#define APP_TASK_PRIORITY 5
#define APP_TASK_CORE_ID 0

static TaskHandle_t app_task_handle = NULL;

esp_err_t app_task_init(void) {
    
    if(xTaskCreatePinnedToCore(app_task, "app_task", APP_TASK_STACK_SIZE, NULL, APP_TASK_PRIORITY, &app_task_handle, APP_TASK_CORE_ID) == pdPASS) {
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
    uint32_t start_time_us = esp_timer_get_time();

    #ifdef SERVO_N20_CONFIG
    ESP_LOGI("APP_TASK", "SERVO_N20_CONFIG defined");
    chandle_valve_cmd(N20_VALVE_CLOSE, 0);
    #elif defined(SERVO_ETH_N2_CONFIG)
    ESP_LOGI("APP_TASK", "SERVO_ETH_N2_CONFIG defined");
    chandle_valve_cmd(ETH_VALVE_CLOSE, 0);
    chandle_valve_cmd(N2_VALVE_CLOSE, 0);
    #elif defined(SOL_ETH_CONFIG)
    ESP_LOGI("APP_TASK", "SOL_ETH_CONFIG defined");
    chandle_valve_cmd(ETH_SOL_CLOSE, 0);
    #elif defined(SOL_N2O_N2_CONFIG)
        chandle_valve_cmd(N20_SOL_OPEN, 0);
        chandle_valve_cmd(N2_SOL_CLOSE, 0);
    ESP_LOGI("APP_TASK", "SOL_N2O_N2_CONFIG defined");
    #endif

    while(1) {
        if(xSemaphoreTake(BoardDataSemaphore, portMAX_DELAY) == pdTRUE) {
            boardData.time_ms = (uint32_t)(esp_timer_get_time() - start_time_us) / 1000;
            xSemaphoreGive(BoardDataSemaphore);
        }
        

        vTaskDelay(1000/TIMESTAMPS_MEASURE_SPS/portTICK_PERIOD_MS);
    }
}