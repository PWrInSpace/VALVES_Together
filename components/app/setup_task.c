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

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "board_config.h"
#include "app_task.h"
// #include "timers_config.h"

#define SETUP_TASK_STACK_SIZE 4096
#define SETUP_TASK_PRIORITY 9
#define SETUP_TASK_CORE_ID 0

#define TAG "SETUP_TASK"

static TaskHandle_t setup_task_handle = NULL;

void setup_task(void *arg) {
    esp_err_t err;

    err = board_config_init();

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Board configuration failed");
        vTaskDelete(NULL);
    }
    
    // Start the app task
    if(app_task_init() != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize app task");
        vTaskDelete(NULL);
    }
    
    // Delete the setup task
    vTaskDelete(NULL);
}

esp_err_t setup_task_init(void) {
    
    // Create the setup task
    if(xTaskCreatePinnedToCore(setup_task, "setup_task", SETUP_TASK_STACK_SIZE, NULL, SETUP_TASK_PRIORITY, &setup_task_handle, SETUP_TASK_CORE_ID) == pdPASS) {
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