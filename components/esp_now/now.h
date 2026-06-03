#ifndef NOW_H
#define NOW_H

#include "esp_crc.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_mac.h"
#include "esp_netif.h"
#include "esp_now.h"
#include "esp_sleep.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "valve_board_config.h"
#include <driver/uart.h>

/**************************  CONFIG INCLUDES  *********************************/

/**************************  DEFINES  *****************************************/

/**************************  INIT *********************************************/

/**************************  PUBLIC VARIABLES  ********************************/

extern const uint8_t adressObc[];
extern bool now_send_data_log_enabled;
extern bool obc_test_data_enabled;

/**************************  CODE *********************************************/

// Init:
esp_err_t nowInit();

// Dodanie peera:
bool nowAddPeer(const uint8_t *address, uint8_t channel);

// Przerwania:
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData,
                int len);
void createNowSendTask();
#endif