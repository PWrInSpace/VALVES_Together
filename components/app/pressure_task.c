#include "pressure_task.h"

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

esp_err_t pressure_task_init(void) {
    
    if(xTaskCreatePinnedToCore(pressure_task, "pressure_task", APP_TASK_STACK_SIZE, NULL, APP_TASK_PRIORITY, &app_task_handle, APP_TASK_CORE_ID) == pdPASS) {
        ESP_LOGI("PRESSURE_TASK", "Pressure task created successfully");
    } else {
        ESP_LOGE("PRESSURE_TASK", "Failed to create pressure task");
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t pressure_task_deinit(void) {
    if (app_task_handle != NULL) {
        vTaskDelete(app_task_handle);
        app_task_handle = NULL;
    }
    
    return ESP_OK;
}

void pressure_task(void *arg) {
    float pressure1 = 0.0;
    float pressure2 = 0.0;

    while(1) {
        if(pressure_manager.Initialized) {
            pressure1 = get_pressure(&pressure_manager.mPressure1);
            pressure2 = get_pressure(&pressure_manager.mPressure2);
            // ESP_LOGI("PRESSURE_TASK", "Pressure1: %.2f kPa, Pressure2: %.2f kPa", pressure1, pressure2);
        }
        else
        {
            ESP_LOGW("PRESSURE_TASK", "Pressure manager not initialized");
            vTaskDelay(pdMS_TO_TICKS(1000));

            continue;
        }
        
        
        if(xSemaphoreTake(BoardDataSemaphore, portMAX_DELAY) == pdTRUE) {
            if(pressure_manager.Initialized) {
                boardData.pressure[0] = pressure1;
                boardData.pressure[1] = pressure2;
            } else {
                ESP_LOGW("PRESSURE_TASK", "Pressure manager not initialized");
            }
            xSemaphoreGive(BoardDataSemaphore);
        } else {
            ESP_LOGE("PRESSURE_TASK", "Failed to take BoardData semaphore");
        }

        vTaskDelay(pdMS_TO_TICKS(10));

    }
}
