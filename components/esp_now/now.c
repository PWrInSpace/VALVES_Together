#include "now.h"
#include "BoardData.h"
#include "commands.h"
#include "servo_control.h"
#include "Solenoid.h"
#include "BoardData.h"
/**************************  PRIVATE INCLUDES  ********************************/

#include <string.h>

/**************************  PRIVATE VARIABLES  *******************************/
// Adres OBC:
const uint8_t adressObc[] = {0x94, 0x54, 0xc5, 0xe8, 0xf1, 0x10}; //dane devkita do testow

/**************************  PRIVATE FUNCTIONS  *******************************/

bool adressCompare(const uint8_t *addr1, const uint8_t *addr2);
void obc_command_handler(const uint8_t *data, int len);

/**************************  CODE *********************************************/

bool nowInit() {

    nvs_flash_init();
    esp_netif_init();
    esp_event_loop_create_default();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_storage(WIFI_STORAGE_RAM);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE); 
    esp_wifi_start();

    if (esp_now_init())
        return false;

    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);

    ESP_LOGI("NOW", "ESP-NOW initialized");

    return true;
}

bool nowAddPeer(const uint8_t *address, uint8_t channel) {

    esp_now_peer_info_t peerInfo = {};

    memcpy(peerInfo.peer_addr, address, 6);
    peerInfo.channel = channel;

    if (esp_now_add_peer(&peerInfo))
        return false;
    return true;
}

// This callback is not used in Broadcast concept. It is left for future ideas.
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
}

void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {

    ESP_LOGI("NOW_DEBUG", "Received data from: %02x:%02x:%02x:%02x:%02x:%02x", 
             info->src_addr[0], info->src_addr[1], info->src_addr[2], 
             info->src_addr[3], info->src_addr[4], info->src_addr[5]);
    if (adressCompare(info->src_addr, adressObc)) {
        if(incomingData != NULL && len == sizeof(DataFromObc)) {
            obc_command_handler(incomingData, len);
        } else {
            ESP_LOGE("NOW", "Received invalid data from OBC");
        }
        

    }


}

bool adressCompare(const uint8_t *addr1, const uint8_t *addr2) {

    for (int8_t i = 0; i < 6; i++) {

        if (addr1[i] != addr2[i])
            return false;
    }

    return true;
}

void obc_command_handler(const uint8_t *data, int len) {
    DataFromObc rxData;
    if (len != sizeof(DataFromObc)) {
        ESP_LOGE("NOW", "Invalid data length from OBC: %d", len);
        return;
    }
    memcpy(&rxData, data, sizeof(DataFromObc));
    moduleData.dataFromObc = rxData;
    ESP_LOGI("ESP-NOW", "Received command: %lu", moduleData.dataFromObc.commandNum);

    switch (moduleData.dataFromObc.commandNum) {
        case N20_VALVE_OPEN:
            #ifdef SERVO_N20_CONFIG
            if((moduleData.dataFromObc.commandArg != 0) && open_servo(N2O_FILL_SERVO, moduleData.dataFromObc.commandArg) != ESP_OK) {
                ESP_LOGE("NOW", "Failed to open N2O_FILL_SERVO");
                moduleData.dataToObc.valve1_state = 0;
            } else {
                moduleData.dataToObc.valve1_state = 1;
            }
            #endif
            break;
        case N20_VALVE_CLOSE:
            #ifdef SERVO_N20_CONFIG
            if(close_servo(N2O_FILL_SERVO) != ESP_OK) {
                ESP_LOGE("NOW", "Failed to close N2O_FILL_SERVO");
                moduleData.dataToObc.valve1_state = 1;

            } else {
                moduleData.dataToObc.valve1_state = 0;
            }
            #endif
            break;
        case ETH_VALVE_OPEN:
            #ifdef SERVO_ETH_N2_CONFIG
            if((moduleData.dataFromObc.commandArg != 0) && open_servo(ETH_FILL_SERVO, moduleData.dataFromObc.commandArg) != ESP_OK) {
                ESP_LOGE("NOW", "Failed to open ETH_FILL_SERVO");
                moduleData.dataToObc.valve1_state = 0;
            }
            else {
                moduleData.dataToObc.valve1_state = 1;
            }
            #endif
            break;
        case ETH_VALVE_CLOSE:
            #ifdef SERVO_ETH_N2_CONFIG
            if(close_servo(ETH_FILL_SERVO) != ESP_OK) {
                ESP_LOGE("NOW", "Failed to close ETH_FILL_SERVO");
                moduleData.dataToObc.valve1_state = 1;
            }
            else {
                moduleData.dataToObc.valve1_state = 0;
            }
            #endif
            break;
        case N2_VALVE_OPEN:
             #ifdef SERVO_ETH_N2_CONFIG
            if((moduleData.dataFromObc.commandArg != 0) && open_servo(N2_FILL_SERVO, moduleData.dataFromObc.commandArg) != ESP_OK) {
                ESP_LOGE("NOW", "Failed to open N2_FILL_SERVO");
                moduleData.dataToObc.valve2_state = 0;
            }
            else {
                moduleData.dataToObc.valve2_state = 1;
            }
            #endif
            break;
        case N2_VALVE_CLOSE:
            #ifdef SERVO_ETH_N2_CONFIG
            if(close_servo(N2_FILL_SERVO) != ESP_OK) {
                ESP_LOGE("NOW", "Failed to close N2_FILL_SERVO");
                moduleData.dataToObc.valve2_state = 1;
            }
            else {
                moduleData.dataToObc.valve2_state = 0;
            }
            #endif
            break;
        case N20_SOL_OPEN:
            #ifdef SOL_N2O_N2_CONFIG
            if(set_valve_state(&valves[N20_FILL_SOL], VALVE_OPEN) != ESP_OK) {
                ESP_LOGE("NOW", "Failed to open N20_FILL_SOL");
                moduleData.dataToObc.valve1_state = 0;
            } else {
                moduleData.dataToObc.valve1_state = 1;
            }
            #endif
        case N20_SOL_CLOSE:
            #ifdef SOL_N2O_N2_CONFIG
            if(set_valve_state(&valves[N20_FILL_SOL], VALVE_CLOSED) != ESP_OK) {
                ESP_LOGE("NOW", "Failed to close N20_FILL_SOL");
                moduleData.dataToObc.valve1_state = 1;
            }
            else {
                moduleData.dataToObc.valve1_state = 0;
            }
            #endif
            break;
        case ETH_SOL_OPEN:
            #ifdef SOL_ETH_CONFIG
            if(set_valve_state(&valves[ETH_FILL_SOL], VALVE_OPEN) != ESP_OK) {
                ESP_LOGE("NOW", "Failed to open ETH_FILL_SOL");
                moduleData.dataToObc.valve1_state = 0;
            }
            else {
                moduleData.dataToObc.valve1_state = 1;
            }
            #endif
            break;
        case ETH_SOL_CLOSE:
            #ifdef SOL_ETH_CONFIG
            if(set_valve_state(&valves[ETH_FILL_SOL], VALVE_CLOSED) != ESP_OK) {
                ESP_LOGE("NOW", "Failed to close ETH_FILL_SOL");
                moduleData.dataToObc.valve1_state = 1;
            }
            else {
                moduleData.dataToObc.valve1_state = 0;
            }
            #endif
            break;
        case N2_SOL_OPEN:
            #ifdef SOL_N2O_N2_CONFIG
            if(set_valve_state(&valves[N2_FILL_SOL], VALVE_OPEN) != ESP_OK) {
                ESP_LOGE("NOW", "Failed to open N2_FILL_SOL");
                moduleData.dataToObc.valve2_state = 0;
            }
            else {
                moduleData.dataToObc.valve2_state = 1;
            }
            #endif
            break;
        case N2_SOL_CLOSE:
            #ifdef SOL_N2O_N2_CONFIG
            if(set_valve_state(&valves[N2_FILL_SOL], VALVE_CLOSED) != ESP_OK) {
                ESP_LOGE("NOW", "Failed to close N2_FILL_SOL");
                moduleData.dataToObc.valve2_state = 1;
            }
            else {
                moduleData.dataToObc.valve2_state = 0;
            }
            #endif
            break;
        case GET_DATA:
            if(xSemaphoreTake(BoardDataSemaphore, 100) == pdTRUE) {
                moduleData.dataToObc.pressure1 = boardData.pressure[0];
                moduleData.dataToObc.pressure2 = boardData.pressure[1];
                xSemaphoreGive(BoardDataSemaphore);
            } else {
                ESP_LOGE("NOW", "Failed to take BoardData semaphore");
                moduleData.dataToObc.pressure1 = 0;
                moduleData.dataToObc.pressure2 = 0;
            }

            ESP_LOGI("NOW", "Sending data to OBC: valve1_state=%d, valve2_state=%d, pressure1=%lu, pressure2=%lu",
                     moduleData.dataToObc.valve1_state, moduleData.dataToObc.valve2_state,
                     moduleData.dataToObc.pressure1, moduleData.dataToObc.pressure2);
            if (esp_now_send(adressObc, (uint8_t*)&moduleData.dataToObc, sizeof(DataToObc)) != ESP_OK) {
                ESP_LOGE("NOW", "Failed to send data to OBC");
            }
            break;
        default:
            ESP_LOGW("NOW", "Unknown command from OBC: %lu", moduleData.dataFromObc.commandNum);
            break;
    }
}
