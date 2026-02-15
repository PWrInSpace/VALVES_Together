// ///===-----------------------------------------------------------------------------------------===//
// ///
// /// Copyright (c) PWr in Space. All rights reserved.
// /// Created: 27.01.2024 by Michał Kos
// ///
// ///===-----------------------------------------------------------------------------------------===//
// ///
// /// \file
// /// This file contains implementation of the system console configuration, including initialization
// /// and available commands for debugging/testing purposes.
// ///===-----------------------------------------------------------------------------------------===//

// #include "esp_log.h"
// #include "esp_system.h"

// #include "console.h"
// #include "console_config.h"
// #include "board_config.h"
// #include "valve_board_config.h"
// #include "valves_control.h"
// #include "commands.h"
// #define TAG "CONSOLE_CONFIG"


// // example function to reset the device
// int reset_device(int argc, char **argv) {
//     ESP_LOGI(TAG, "Resetting device...");
//     esp_restart();
//     return 0;
// }

// int open_valve(int argc, char **argv) {
//     if (argc != 3) {
//         ESP_LOGE(TAG, "Invalid number of arguments. Usage: open_valve <valve_id> <time_ms>");
//         return -1;
//     }
//     int valve_id = atoi(argv[1]);
//     int time_ms = atoi(argv[2]);

//     #ifdef SERVO_N20_CONFIG
//     if (valve_id == 1) {
//         chandle_valve_cmd(N20_VALVE_OPEN, time_ms);
//         return 0;
//     }
//     else if (valve_id == 2) {
//         ESP_LOGE(TAG, "N2_FILL_SERVO not available in SERVO_N20_CONFIG");
//         return -1;
//     }
//     #endif

//     #ifdef SERVO_ETH_N2_CONFIG
//     if (valve_id == 1) {
//         chandle_valve_cmd(ETH_VALVE_OPEN, time_ms);
//         return 0;
//     }
//     else if (valve_id == 2) {
//         chandle_valve_cmd(N2_VALVE_OPEN, time_ms);
//         return 0;
//     }
//     #endif

//     #ifdef SOL_ETH_CONFIG
//     if (valve_id == 1) {
//         chandle_valve_cmd(ETH_SOL_OPEN, time_ms);
//         return 0;
//     }
//     else if (valve_id == 2) {
//         ESP_LOGE(TAG, "N2_FILL_SERVO not available in SOL_ETH_CONFIG");
//         return -1;
//     }
//     #endif

//     #ifdef SOL_N2O_N2_CONFIG
//     if (valve_id == 1) {
//         chandle_valve_cmd(N20_SOL_OPEN, time_ms);
//         return 0;
//     }
//     else if (valve_id == 2) {
//         chandle_valve_cmd(N2_SOL_OPEN, time_ms);
//         return 0;
//     }
//     #endif
    

//     ESP_LOGE(TAG, "Invalid valve_id. Available valves depend on the board configuration.");
//     return -1;

// }    

// int print_data(int argc, char **argv) {
//     chandle_valve_cmd(GET_DATA, 0);
//     return 0;
// }

// int close_valve(int argc, char **argv) {
//     if (argc != 2) {
//         ESP_LOGE(TAG, "Invalid number of arguments. Usage: close_solenoid <solenoid_id>");
//         return -1;
//     }
//     int valve_id = atoi(argv[1]);

//     #ifdef SOL_N2O_N2_CONFIG
//     if (valve_id == 1) {
//         chandle_valve_cmd(N20_SOL_CLOSE, 0);
//         return 0;
//     }
//     else if (valve_id == 2) {
//         chandle_valve_cmd(N2_SOL_CLOSE, 0);
//         return 0;
//     }
//     #endif

//     #ifdef SOL_ETH_CONFIG
//     if (valve_id == 1) {
//         chandle_valve_cmd(ETH_SOL_CLOSE, 0);
//         return 0;
//     }
//     else if (valve_id == 2) {
//         ESP_LOGE(TAG, "N2_FILL_SOL not available in SOL_ETH_CONFIG");
//         return -1;
//     }
//     #endif

//     #ifdef SERVO_N20_CONFIG
//     if (valve_id == 1)
//     {
//         chandle_valve_cmd(N20_VALVE_CLOSE, 0);
//         return 0;
//     }
//     else if (valve_id == 2)
//     {
//         ESP_LOGE(TAG, "N2_FILL_SERVO not available in SERVO_N20_CONFIG");
//         return -1;
//     }
//     #endif

//     #ifdef SERVO_ETH_N2_CONFIG
//     if (valve_id == 1)
//     {
//         chandle_valve_cmd(ETH_VALVE_CLOSE, 0);
//         return 0;
//     }
//     else if (valve_id == 2)
//     {
//         chandle_valve_cmd(N2_VALVE_CLOSE, 0);
//         return 0;
//     }
//     #endif

//     ESP_LOGE(TAG, "Invalid solenoid_id. Available solenoids depend on the board configuration.");
//     return -1;
// }
//  // Place for the console configuration

//  static esp_console_cmd_t cmd [] = {
//  // example command:
//  // cmd     help description   hint  function      args  func_context context
//  {"reset", "Reset the device", NULL, reset_device, NULL, NULL, NULL},
//     {"open_valve", "Open a valve. Usage: open_valve <valve_id> <time_ms> for servo valves or open_valve <valve_id> for solenoid valves", NULL, open_valve, NULL, NULL, NULL},
//     {"print_data", "Print current valve states and pressure readings", NULL, print_data, NULL, NULL, NULL},
//     {"close_valve", "Close a solenoid valve. Usage: close_solenoid <solenoid_id>", NULL, close_valve, NULL, NULL, NULL},
//  };

// esp_err_t console_config_init() {
//     esp_err_t ret;
//     ret = console_init();
//     ret = console_register_commands(cmd, sizeof(cmd) / sizeof(cmd[0]));
//     if (ret != ESP_OK) {
//         ESP_LOGE(TAG, "%s", esp_err_to_name(ret));
//         return ret;
//     }
//     return ret;
// }