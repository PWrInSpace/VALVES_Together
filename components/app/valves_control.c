#include "valves_control.h"
#include "BoardData.h"
#include "commands.h"
#include "Solenoid.h"
#include "esp_log.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "servo_control.h"
#include "system_timer.h"

void chandle_valve_cmd(uint8_t cmd, int time_ms) {

    ESP_LOGI("VALVES_CONTROL", "Handling valve command: %d with time: %d ms", cmd, time_ms);

    switch (cmd) {
        case N20_VALVE_OPEN:
            #ifdef SERVO_N20_CONFIG
            if( open_servo(N2O_FILL_SERVO, time_ms) != ESP_OK) {
                ESP_LOGE("VALVES_CONTROL", "Failed to open N2O_FILL_SERVO");
                valve1_state = 0;
            } else {
                valve1_state = 1;
            }
            #endif
            break;
        case N20_VALVE_CLOSE:
            #ifdef SERVO_N20_CONFIG
            if(close_servo(N2O_FILL_SERVO) != ESP_OK) {
                ESP_LOGE("VALVES_CONTROL", "Failed to close N2O_FILL_SERVO");
                valve1_state = 1;

            } else {
                valve1_state = 0;
            }
            #endif
            break;
        case ETH_VALVE_OPEN:
            #ifdef SERVO_ETH_N2_CONFIG
            if( open_servo(ETH_FILL_SERVO, time_ms) != ESP_OK) {
                ESP_LOGE("VALVES_CONTROL", "Failed to open ETH_FILL_SERVO");
                valve1_state = 0;
            }
            else {
                valve1_state = 1;
            }
            #endif
            break;
        case ETH_VALVE_CLOSE:
            #ifdef SERVO_ETH_N2_CONFIG
            if(close_servo(ETH_FILL_SERVO) != ESP_OK) {
                ESP_LOGE("VALVES_CONTROL", "Failed to close ETH_FILL_SERVO");
                valve1_state = 1;
            }
            else {
                valve1_state = 0;
            }
            #endif
            break;
        case N2_VALVE_OPEN:
             #ifdef SERVO_ETH_N2_CONFIG
            if( open_servo(N2_FILL_SERVO, time_ms) != ESP_OK) {
                ESP_LOGE("VALVES_CONTROL", "Failed to open N2_FILL_SERVO");
                valve2_state = 0;
            }
            else {
                valve2_state = 1;
            }
            #endif
            break;
        case N2_VALVE_CLOSE:
            #ifdef SERVO_ETH_N2_CONFIG
            if(close_servo(N2_FILL_SERVO) != ESP_OK) {
                ESP_LOGE("VALVES_CONTROL", "Failed to close N2_FILL_SERVO");
                valve2_state = 1;
            }
            else {
                valve2_state = 0;
            }
            #endif
            break;
        case N20_SOL_OPEN:
            #ifdef SOL_N2O_N2_CONFIG
            if(close_sol_time(valves[N20_FILL_SOL].name, time_ms) != ESP_OK) {
                ESP_LOGE("VALVES_CONTROL", "Failed to open N20_FILL_SOL");
                valve1_state = 1;
            } else {
                valve1_state = 0;
            }
            #endif
            ESP_LOGI("VALVES_CONTROL", "Handled N20_SOL_OPEN command DUPA");
            break;
        case N20_SOL_CLOSE:
            #ifdef SOL_N2O_N2_CONFIG
            if(open_solenoid(valves[N20_FILL_SOL].name, time_ms) != ESP_OK) {
                ESP_LOGE("VALVES_CONTROL", "Failed to close N20_FILL_SOL");
                valve1_state = 0;
            }
            else {
                valve1_state = 1;
            }
            #endif
            break;
        case ETH_SOL_OPEN:
            #ifdef SOL_ETH_CONFIG
            if(open_solenoid(valves[ETH_FILL_SOL].name, time_ms) != ESP_OK) {
                ESP_LOGE("VALVES_CONTROL", "Failed to open ETH_FILL_SOL");
                valve1_state = 0;
            }
            else {
                valve1_state = 1;
            }
            #endif
            break;
        case ETH_SOL_CLOSE:
            #ifdef SOL_ETH_CONFIG
            if(close_sol(valves[ETH_FILL_SOL].name) != ESP_OK) {
                ESP_LOGE("VALVES_CONTROL", "Failed to close ETH_FILL_SOL");
                valve1_state = 1;
            }
            else {
                valve1_state = 0;
            }
            #endif
            break;
        case N2_SOL_OPEN:
            #ifdef SOL_N2O_N2_CONFIG
            if(open_solenoid(valves[N2_FILL_SOL].name, time_ms) != ESP_OK) {
                ESP_LOGE("VALVES_CONTROL", "Failed to open N2_FILL_SOL");
                valve2_state = 0;
            }
            else {
                valve2_state = 1;
            }
            #endif
            break;
        case N2_SOL_CLOSE:
            #ifdef SOL_N2O_N2_CONFIG
            if(close_sol(valves[N2_FILL_SOL].name) != ESP_OK) {
                ESP_LOGE("VALVES_CONTROL", "Failed to close N2_FILL_SOL");
                valve2_state = 1;
            }
            else {
                valve2_state = 0;
            }
            #endif
            break;
        case VALVE_DZIDA:
            ESP_LOGI("VALVES_CONTROL", "DZIDA COMMAND RECEIVED");
            #ifdef SERVO_N20_CONFIG
                vTaskDelay(pdMS_TO_TICKS(100));
                chandle_valve_cmd(N20_VALVE_OPEN, 0);
            #endif

            #ifdef SERVO_ETH_N2_CONFIG
                vTaskDelay(pdMS_TO_TICKS(220));
                chandle_valve_cmd(ETH_VALVE_OPEN, 0);

                
            #endif

            break;
        default:
            ESP_LOGW("VALVES_CONTROL", "Unknown command: %lu", moduleData.dataFromObc.commandNum);
            break;
    }
}

void chandle_valve_cmd_angle(uint8_t cmd, int time_ms, int angle) {

    ESP_LOGI("VALVES_CONTROL", "Handling valve command: %d with time: %d ms and angle: %d", cmd, time_ms, angle);

    switch (cmd) {
        case N20_VALVE_OPEN:
            #ifdef SERVO_N20_CONFIG
            if( move_servo(N2O_FILL_SERVO, angle, time_ms) != ESP_OK) {
                ESP_LOGE("VALVES_CONTROL", "Failed to open N2O_FILL_SERVO");
                valve1_state = 0;
            } else {
                valve1_state = 1;
            }
            #endif
            break;
        case ETH_VALVE_OPEN:
            #ifdef SERVO_ETH_N2_CONFIG
            if( move_servo(ETH_FILL_SERVO, angle, time_ms) != ESP_OK) {
                ESP_LOGE("VALVES_CONTROL", "Failed to open ETH_FILL_SERVO");
                valve1_state = 0;
            }
            else {
                valve1_state = 1;
            }
            #endif
            break;
        case N2_VALVE_OPEN:
             #ifdef SERVO_ETH_N2_CONFIG
            if( move_servo(N2_FILL_SERVO, angle, time_ms) != ESP_OK) {
                ESP_LOGE("VALVES_CONTROL", "Failed to open N2_FILL_SERVO");
                valve2_state = 0;
            }
            else {
                valve2_state = 1;
            }
            #endif
            break;
        default:
            ESP_LOGW("VALVES_CONTROL", "Unknown command: %lu", moduleData.dataFromObc.commandNum);
            break;
    }
}