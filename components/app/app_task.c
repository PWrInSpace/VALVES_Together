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
        #if (defined(PRESSURE_SENSOR_70kPa) || defined(PRESSURE_SENSOR_300kPa)) 
        // Read pressures
        float pressure1 = 0;

        if(pressure_manager.Initialized) {
            for(int counter = 0; counter < 3; counter++) {
                pressure1 += get_pressure(&pressure_manager.mPressure1);
                vTaskDelay(20/portTICK_PERIOD_MS);
            }
            pressure1 /= 3;
        } else {
            vTaskDelay(50/portTICK_PERIOD_MS);
            pressure1 = 0;
        }

        
        if(xSemaphoreTake(BoardDataSemaphore, portMAX_DELAY) == pdTRUE) {
            if(pressure_manager.Initialized) {
                boardData.pressure[0] = pressure1;
                // ESP_LOGI("APP_TASK", "Pressure1: %.2f kPa", boardData.pressure[0]);
            } else {
                ESP_LOGW("APP_TASK", "Pressure manager not initialized");
            }
            xSemaphoreGive(BoardDataSemaphore);
        } else {
            ESP_LOGE("APP_TASK", "Failed to take BoardData semaphore");
        }
        // get_voltage(&mVoltage);
        #else
        vTaskDelay(1000/portTICK_PERIOD_MS);
        #endif
        // vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}