///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 06.05.2025 by Szymon Rzewuski
///
///===-----------------------------------------------------------------------------------------===//
///
/// \file
/// This file contains implementation of the system configuration including mcu config, can api config
/// and hardware config to be implemented by user
///===-----------------------------------------------------------------------------------------===//

#include "board_config.h"

#include "driver/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "console_config.h"
#include "mcu_spi_config.h"
#include "timers_config.h"

#define TAG "BOARD_CONFIG"

void _led_delay(uint32_t _ms) {
    vTaskDelay(_ms / portTICK_PERIOD_MS);
}

board_config_t config = {
    .board_name = "VALVES_Together_BOARD", //CHANGE TO REAL BOARD NAME
};

esp_err_t board_config_init(void) {

    esp_err_t err;
    
    err = console_config_init();

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Console initialization failed");
        return err;
    }

    err = mcu_spi_init();

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "SPI initialization failed");
        return err;
    }

    if(!timers_init())
    {
        ESP_LOGE(TAG, "Failed to initialize timers");
        return ESP_FAIL;
    }


    return ESP_OK;

    //*********** ADD HARDWARE CONFIGURATION HERE ***********//

    
}