#include "now.h"
#include "BoardData.h"
#include "commands.h"
#include "servo_control.h"
#include "Solenoid.h"
#include "BoardData.h"
#include "valve_board_config.h"
#include "valves_control.h"

/**************************  PRIVATE INCLUDES  ********************************/

#include <string.h>

/**************************  PRIVATE VARIABLES  *******************************/
// Adres OBC:
const uint8_t adressObc[] = {0x04, 0x20, 0x04, 0x20, 0x04, 0x20}; //dane devkita do testow

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

    #ifdef SERVO_N20_CONFIG
    uint8_t custom_mac[6] = {0xBA, 0x11, 0x22, 0x33, 0x44, 0x55};
    #elif defined(SERVO_ETH_N2_CONFIG)
    uint8_t custom_mac[6] = {0xBA, 0x11, 0x22, 0x33, 0x44, 0x66};
    #elif defined(SOL_ETH_CONFIG)
    uint8_t custom_mac[6] = {0xBA, 0x11, 0x22, 0x33, 0x44, 0x77};
    #elif defined(SOL_N2O_N2_CONFIG)
    uint8_t custom_mac[6] = {0xBA, 0x11, 0x22, 0x33, 0x44, 0x88};
    #endif

    ESP_ERROR_CHECK(esp_wifi_set_mac(WIFI_IF_STA, custom_mac));
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

    // ESP_LOGI("NOW_DEBUG", "Received data from: %02x:%02x:%02x:%02x:%02x:%02x", 
    //          info->src_addr[0], info->src_addr[1], info->src_addr[2], 
    //          info->src_addr[3], info->src_addr[4], info->src_addr[5]);



    if (adressCompare(info->src_addr, adressObc)) {

        // ESP_LOGI("NOW", "Data received from OBC in length: %d", len);


        if(incomingData == NULL)
        {
            ESP_LOGE("NOW", "Received NULL data from OBC");
            return;
        }


        if (len == sizeof(moduleData.obcState))
        {
            memcpy((void*) &moduleData.obcState, (uint16_t *)incomingData, sizeof(moduleData.obcState));
            // ESP_LOGI("NOW", "OBC State updated to: %d", moduleData.obcState);
        }
        else if(len == sizeof(DataFromObc))  {
            ESP_LOGI("NOW", "Data received from OBC");

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
    chandle_valve_cmd(moduleData.dataFromObc.commandNum, moduleData.dataFromObc.commandArg);
}

void now_send_data_to_obc(void *arg) {
    while (1) {
        if (esp_now_send(adressObc, (uint8_t *)&moduleData.dataToObc, sizeof(DataToObc)) != ESP_OK) {
            ESP_LOGE("NOW", "Error sending data to OBC");
        }

        ESP_LOGI("NOW", "OBC state: %d", moduleData.obcState);
        ESP_LOGI("NOW", "Data sent to OBC: valve1_state=%d, valve2_state=%d, temp1=%d, temp2=%d, temp3=%d, pres1=%d, pres2=%d, batt=%.2f",
                 moduleData.dataToObc.valve1_state,
                 moduleData.dataToObc.valve2_state,
                 moduleData.dataToObc.temperature1,
                 moduleData.dataToObc.temperature2,
                 moduleData.dataToObc.temperature3,
                 moduleData.dataToObc.pressure1,
                 moduleData.dataToObc.pressure2,
                 moduleData.dataToObc.battery_voltage);

        vTaskDelay(pdMS_TO_TICKS(moduleData.stateTimes[moduleData.obcState]));
    }
}

void createNowSendTask()
{
    xTaskCreatePinnedToCore(
        now_send_data_to_obc,   
        "now_send_task",                        
        4096,                                   
        NULL,                                   
        5,                                      
        NULL,                                   
        1                                      
    );
}


