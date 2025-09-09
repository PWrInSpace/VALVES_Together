#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_err.h"

#include "board_config.h"
#include "setup_task.h"
#include "app_task.h"
#include "timers_config.h"
#include "pressure_sensor.h"


#define TAG "APP"

extern board_config_t config;

void app_main(void) {
    
    // CONFIGURE THE MESSAGE

    ESP_LOGI(TAG, "%s VALVES_Together board starting", config.board_name);
    
    if(setup_task_init() != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize setup task");
        return;
    }

    start_test_timer();

    while(1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

/* main.c - ESP-NOW SLAVE (odbiorca) */
// #include <stdio.h>
// #include <string.h>
// #include "esp_wifi.h"
// #include "esp_now.h"
// #include "esp_event.h"
// #include "esp_log.h"
// #include "nvs_flash.h"
// #include "esp_mac.h"

// static const char *TAG = "SLAVE";
// #define WIFI_CHANNEL 1  // Ustaw ten sam kanał na obu urządzeniach

// void on_data_recv(const esp_now_recv_info_t *mac_addr, const uint8_t *data, int len) {
//     char macStr[18];
//     snprintf(macStr, sizeof(macStr),
//              "%02X:%02X:%02X:%02X:%02X:%02X",
//              mac_addr[0], mac_addr[1], mac_addr[2],
//              mac_addr[3], mac_addr[4], mac_addr[5]);
//     ESP_LOGI(TAG, "Odebrano od %s: %.*s", macStr, len, (char *)data);
// }

// void app_main(void) {
//     ESP_ERROR_CHECK(nvs_flash_init());
//     ESP_ERROR_CHECK(esp_netif_init());
//     ESP_ERROR_CHECK(esp_event_loop_create_default());

//     // Tryb STA
//     wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
//     ESP_ERROR_CHECK(esp_wifi_init(&cfg));
//     ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
//     ESP_ERROR_CHECK(esp_wifi_start());
//     ESP_ERROR_CHECK(esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE));


//     // Inicjalizacja ESP-NOW
//     ESP_ERROR_CHECK(esp_now_init());
//     esp_now_peer_info_t peerInfo = {0};
// memset(&peerInfo, 0, sizeof(peerInfo));
// for (int i = 0; i < 6; i++) peerInfo.peer_addr[i] = 0xFF;
// peerInfo.channel = 1;
// peerInfo.encrypt = false;
// esp_now_add_peer(&peerInfo);
//     ESP_ERROR_CHECK(esp_now_register_recv_cb(on_data_recv));

//     uint8_t mac[6];
// esp_read_mac(mac, ESP_MAC_WIFI_STA);
// ESP_LOGI("SLAVE", "MAC: %02X:%02X:%02X:%02X:%02X:%02X",
//          mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);


//     ESP_LOGI(TAG, "SLAVE gotowy na kanale %d", WIFI_CHANNEL);
// }
