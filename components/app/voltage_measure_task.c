#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "BoardData.h"
#include "voltage_measure.h"
#include "commands.h"
#include "esp_timer.h"
#include "voltage_measure_task.h"
#include "valve_board_config.h"

#define APP_TASK_STACK_SIZE 8192
#define APP_TASK_PRIORITY 5
#define APP_TASK_CORE_ID 0

static TaskHandle_t voltage_task_handle = NULL;
esp_err_t voltage_task_init(void) {
    
    if(xTaskCreatePinnedToCore(voltage_task, "voltage_task", APP_TASK_STACK_SIZE, NULL, APP_TASK_PRIORITY, &voltage_task_handle, APP_TASK_CORE_ID) == pdPASS) {
        ESP_LOGI("VOLTAGE_TASK", "Voltage task created successfully");
    } else {
        ESP_LOGE("VOLTAGE_TASK", "Failed to create voltage task");
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t voltage_task_deinit(void) {
    if (voltage_task_handle != NULL) {
        vTaskDelete(voltage_task_handle);
        voltage_task_handle = NULL;
    }
    
    return ESP_OK;
}

void voltage_task(void *arg) {
    while(1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        float battery_voltage = get_voltage(0);
        if(xSemaphoreTake(BoardDataSemaphore, portMAX_DELAY) == pdTRUE) {
            boardData.battery_voltage = battery_voltage;
            xSemaphoreGive(BoardDataSemaphore);
        }
        
    }
}
