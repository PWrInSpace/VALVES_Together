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

    ESP_LOGI(TAG, "Setup task initialized");

    // start_test_timer();

    // while(1) {
    //     vTaskDelay(1000 / portTICK_PERIOD_MS);
    // }
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


// #include <stdio.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "esp_log.h"
// #include "driver/gpio.h"
// #include "driver/sdspi_host.h"
// #include "driver/spi_common.h"
// #include "esp_vfs_fat.h"
// #include "sdmmc_cmd.h"

// #define MOUNT_POINT "/sdcard"
// #define PIN_CS 5
// static const char *TAG = "SD_TEST";

// void app_main(void)
// {
//     ESP_LOGI(TAG, "Starting SD SPI test...");

//     // --- Softowy pull-up na CS ---
//     gpio_config_t io_conf = {
//         .pin_bit_mask = (1ULL << PIN_CS),
//         .mode = GPIO_MODE_OUTPUT,
//         .pull_up_en = GPIO_PULLUP_DISABLE,  // brak fizycznego pull-up
//         .pull_down_en = GPIO_PULLDOWN_DISABLE,
//         .intr_type = GPIO_INTR_DISABLE
//     };
//     gpio_config(&io_conf);
//     gpio_set_level(PIN_CS, 1); // ustaw HIGH jako softowy pull-up

//     // --- Konfiguracja SPI ---
//     sdmmc_host_t host = SDSPI_HOST_DEFAULT();
//     host.slot = SPI3_HOST;
//     host.max_freq_khz = 4000;
//     spi_bus_config_t bus_cfg = {
//         .mosi_io_num = 23,
//         .miso_io_num = 19,
//         .sclk_io_num = 18,
//         .quadwp_io_num = -1,
//         .quadhd_io_num = -1,
//         .max_transfer_sz = 4000,
//     };
//     ESP_ERROR_CHECK(spi_bus_initialize(host.slot, &bus_cfg, SPI_DMA_CH_AUTO));

//     // Konfiguracja SD
//     sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
//     slot_config.gpio_cs = PIN_CS;
//     slot_config.host_id = host.slot;

//     sdmmc_card_t *card;
//     esp_vfs_fat_sdmmc_mount_config_t mount_config = {
//         .format_if_mount_failed = false,
//         .max_files = 5
//     };

//     // --- Montowanie karty SD ---
//     esp_err_t ret = esp_vfs_fat_sdspi_mount(MOUNT_POINT, &host, &slot_config, &mount_config, &card);
//     if (ret != ESP_OK) {
//         ESP_LOGE(TAG, "Failed to mount SD card. Error: %s", esp_err_to_name(ret));
//         return;
//     }

//     ESP_LOGI(TAG, "SD card mounted successfully!");
//     ESP_LOGI(TAG, "Card name: %s", card->cid.name);
//     ESP_LOGI(TAG, "Card type: %d", card->cid.oem_id);
//     ESP_LOGI(TAG, "Card capacity: %llu MB", ((uint64_t)card->csd.capacity * card->csd.sector_size) / (1024 * 1024));

//     // Testowy zapis pliku
//     FILE *f = fopen(MOUNT_POINT"/test.txt", "w");
//     if (f) {
//         fprintf(f, "SD card test OK\n");
//         fclose(f);
//         ESP_LOGI(TAG, "Test file written successfully!");
//     } else {
//         ESP_LOGE(TAG, "Failed to open test file for writing");
//     }

//     // Odmontowanie
//     esp_vfs_fat_sdcard_unmount(MOUNT_POINT, card);
//     ESP_LOGI(TAG, "SD card unmounted");
// }
