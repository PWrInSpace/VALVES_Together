///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 27.01.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//
///
/// \file
/// This file contains implementation of the system console configuration, including initialization
/// and available commands for debugging/testing purposes.
///===-----------------------------------------------------------------------------------------===//

#include "esp_log.h"
#include "esp_system.h"

#include "console.h"
#include "console_config.h"
#include "board_config.h"
#include "valve_board_config.h"
#include "valves_control.h"
#include "commands.h"
#include "i2c_scan.h"
#include "ltc4162.h"
#include "buzzer.h"

#define TAG "CONSOLE_CONFIG"


// example function to reset the device
int reset_device(int argc, char **argv) {
    ESP_LOGI(TAG, "Resetting device...");
    esp_restart();
    return 0;
}

int run_i2c_scan(int argc, char **argv) {
    i2c_scan();
    return 0;
}

int run_ltc4162_monitor(int argc, char **argv) {
    ltc4162_debug_monitor();
    return 0;
}

int play_imperial_march(int argc, char **argv) {
    imperial_march();
    return 0;
}

int play_ode_to_joy(int argc, char **argv) {
    ode_to_joy();
    return 0;
}

 // Place for the console configuration

 static esp_console_cmd_t cmd [] = {
 // example command:
 // cmd     help description   hint  function      args  func_context context
 {"reset", "Reset the device", NULL, reset_device, NULL, NULL, NULL},
 {"i2c_scan", "Scan the I2C bus for devices", NULL, run_i2c_scan, NULL, NULL, NULL},
 {"ltc_monitor", "Run LTC4162 debug monitor", NULL, run_ltc4162_monitor, NULL, NULL, NULL},
    {"play_imperial_march", "Play the Imperial March on the buzzer", NULL, play_imperial_march, NULL, NULL, NULL},
    {"play_ode_to_joy", "Play Ode to Joy on the buzzer", NULL, play_ode_to_joy, NULL, NULL, NULL},
 };

esp_err_t console_config_init() {
    esp_err_t ret;
    ret = console_init();
    ret = console_register_commands(cmd, sizeof(cmd) / sizeof(cmd[0]));
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "%s", esp_err_to_name(ret));
        return ret;
    }
    return ret;
}