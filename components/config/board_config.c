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
#include "now.h"
#include "solenoid_config.h"
#include "servo_config.h"
#include "pressure_sensor.h"

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

    if(nowInit())
    {
        nowAddPeer(adressObc, 1);
        uint8_t mac[6];
        esp_wifi_get_mac(ESP_IF_WIFI_STA, mac);
        ESP_LOGI("MAC address", "MAC address: %02x:%02x:%02x:%02x:%02x:%02x", 
                 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]); 
    }
    else
    {
        ESP_LOGE(TAG, "ESP-NOW initialization failed");
        return ESP_FAIL;
    }

    err = valves_init();
    if (err!=ESP_OK)
    {
        ESP_LOGE(TAG, "Valves initialization failed");
        return err;
    }

    err = init_multiple_servos();
    if(err!=ESP_OK)
    {
        ESP_LOGE(TAG, "Servo configuration failed");
        vTaskDelete(NULL);
    }
    
    if(!pressure_sensors_init()) {
        ESP_LOGE(TAG, "Pressure sensors initialization failed");
        return ESP_FAIL;
    }

    return ESP_OK;

    //*********** ADD HARDWARE CONFIGURATION HERE ***********//

    
}