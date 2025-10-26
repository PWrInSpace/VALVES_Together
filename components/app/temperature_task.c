#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "BoardData.h"
#include "temperature_task.h"
#include "commands.h"
#include "esp_timer.h"
#include "valve_board_config.h"
#include "voltage_measure.h"

#define APP_TASK_STACK_SIZE 8192
#define APP_TASK_PRIORITY 5
#define APP_TASK_CORE_ID 0

static TaskHandle_t temperature_task_handle = NULL;
esp_err_t temperature_task_init(void) {
    
    if(xTaskCreatePinnedToCore(temperature_task, "temperature_task", APP_TASK_STACK_SIZE, NULL, APP_TASK_PRIORITY, &temperature_task_handle, APP_TASK_CORE_ID) == pdPASS) {
        ESP_LOGI("TEMPERATURE_TASK", "Temperature task created successfully");
    } else {
        ESP_LOGE("TEMPERATURE_TASK", "Failed to create temperature task");
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t temperature_task_deinit(void) {
    if (temperature_task_handle != NULL) {
        vTaskDelete(temperature_task_handle);
        temperature_task_handle = NULL;
    }
    
    return ESP_OK;
}

void temperature_task(void *arg) {
    while(1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        float thermistor1_temp = get_voltage(1);
        float thermistor2_temp = get_voltage(2);
        if(xSemaphoreTake(BoardDataSemaphore, pdMS_TO_TICKS(50)) == pdTRUE) {
            boardData.thermistor_temp[0] = thermistor1_temp;
            boardData.thermistor_temp[1] = thermistor2_temp;
            xSemaphoreGive(BoardDataSemaphore);
        }
        /*
        THERMOCOUPLE TODO
        */
        
    }
}