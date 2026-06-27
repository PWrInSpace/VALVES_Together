#include "now.h"
#include "BoardData.h"
#include "Solenoid.h"
#include "auto_vent_task.h"
#include "commands.h"
#include "servo_control.h"
#include "valve_board_config.h"
#include "valves_control.h"

/**************************  PRIVATE INCLUDES  ********************************/

#include <math.h>
#include <string.h>

/**************************  PRIVATE VARIABLES  *******************************/
// Adres OBC:
const uint8_t addressObc[] = {0x04, 0x20, 0x04,
                             0x20, 0x04, 0x20}; // dane devkita do testow

bool now_send_data_log_enabled = false;
bool obc_test_data_enabled = false;

static const float OBC_TEST_PRESSURE1 = 69.69f;
static const float OBC_TEST_PRESSURE2 = 88.88f;
static const float OBC_TEST_BATTERY_VOLTAGE = 12.34f;
static const float OBC_TEST_CONSUMPTION = 1.23f;
static const float OBC_TEST_CHARGER_TEMPERATURE = 45.67f;
static const int16_t OBC_TEST_TEMPERATURE = 25;
static const bool OBC_TEST_IS_CHARGING = true;

/**************************  PRIVATE FUNCTIONS  *******************************/

bool addressCompare(const uint8_t *addr1, const uint8_t *addr2);
void obc_command_handler(const uint8_t *data, int len);

/**************************  CODE *********************************************/

// static int dupa = 1;

esp_err_t nowInit() {

  nvs_flash_init();
  esp_netif_init();
  esp_event_loop_create_default();
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&cfg);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_MODE_STA);

  // #ifdef SERVO_N20_CONFIG
  // uint8_t custom_mac[6] = {0xBA, 0x11, 0x22, 0x33, 0x44, 0x55};
  // #elif defined(SERVO_ETH_N2_CONFIG)
  // uint8_t custom_mac[6] = {0xBA, 0x11, 0x22, 0x33, 0x44, 0x66};
  // #elif defined(SOL_ETH_CONFIG)
  // uint8_t custom_mac[6] = {0xBA, 0x11, 0x22, 0x33, 0x44, 0x77};
  // #elif defined(SOL_N2O_N2_CONFIG)
  // uint8_t custom_mac[6] = {0xBA, 0x11, 0x22, 0x33, 0x44, 0x88};
  // #endif

#ifdef SERVO_N20_CONFIG
  uint8_t custom_mac[6] = {0xBA, 0x11, 0x22, 0x33, 0x44, 0x55};
#elif defined(SOL_N2_CONFIG)
  uint8_t custom_mac[6] = {0xBA, 0x11, 0x22, 0x33, 0x44, 0x66};
#elif defined(SOL_N20_SERVO_ETH_CONFIG)
  uint8_t custom_mac[6] = {0xBA, 0x11, 0x22, 0x33, 0x44, 0x77};
#elif defined(SOL_ETH_CONFIG)
  uint8_t custom_mac[6] = {0xBA, 0x11, 0x22, 0x33, 0x44, 0x88};
#endif

  ESP_ERROR_CHECK(esp_wifi_set_mac(WIFI_IF_STA, custom_mac));
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
  esp_wifi_start();

  if (esp_now_init())
    return ESP_FAIL;

  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

  ESP_LOGI("NOW", "ESP-NOW initialized");

  return ESP_OK;
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
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {}

void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData,
                int len) {

  // ESP_LOGI("NOW_DEBUG", "Received data from: %02x:%02x:%02x:%02x:%02x:%02x",
  //          info->src_addr[0], info->src_addr[1], info->src_addr[2],
  //          info->src_addr[3], info->src_addr[4], info->src_addr[5]);

  // ESP_LOGI("NOW", "Received data from OBC");

  if (addressCompare(info->src_addr, addressObc)) {

    // ESP_LOGI("NOW", "Data received from OBC in length: %d", len);

    if (incomingData == NULL) {
      ESP_LOGE("NOW", "Received NULL data from OBC");
      return;
    }

    if (len == sizeof(moduleData.obcState)) {
      memcpy((void *)&moduleData.obcState, (uint16_t *)incomingData,
             sizeof(moduleData.obcState));
      // ESP_LOGI("NOW", "OBC State updated to: %d", moduleData.obcState);
    } else if (len == sizeof(DataFromObc) ||
               len == sizeof(DataFromObc2)) { // +1 for alignment{
      ESP_LOGI("NOW", "Data received from OBC");

      obc_command_handler(incomingData, len);
    } else {
      ESP_LOGE("NOW", "Received invalid data from OBC");
    }
  }
}

bool addressCompare(const uint8_t *addr1, const uint8_t *addr2) {

  for (int8_t i = 0; i < 6; i++) {

    if (addr1[i] != addr2[i])
      return false;
  }

  return true;
}

void obc_command_handler(const uint8_t *data, int len) {
  DataFromObc rxData;
  DataFromObc2 rxData2;
  if (len != sizeof(DataFromObc) && len != sizeof(DataFromObc2)) {
    ESP_LOGE("NOW", "Invalid data length from OBC: %d", len);
    return;
  }
  // if (sizeof(DataFromObc) == len) {
  //   ESP_LOGI("NOW", "Received DataFromObc structure");
  // } else if (sizeof(DataFromObc2) == len) {
  //   ESP_LOGI("NOW", "Received DataFromObc2 structure");
  // }
  if (sizeof(DataFromObc) == len) {
    memcpy(&rxData, data, sizeof(DataFromObc));
    moduleData.dataFromObc = rxData;
    ESP_LOGI("ESP-NOW", "Received command: %lu with arg: %ld",
             moduleData.dataFromObc.commandNum,
             moduleData.dataFromObc.commandArg);
    if (moduleData.dataFromObc.commandNum == OBC_TEST_COMMAND) {
      obc_test_data_enabled = (moduleData.dataFromObc.commandArg != 0);
      ESP_LOGI("NOW", "OBC test data %s",
               obc_test_data_enabled ? "enabled" : "disabled");
      return;
    }
    chandle_valve_cmd((uint8_t)moduleData.dataFromObc.commandNum,
                      moduleData.dataFromObc.commandArg);
    return;
  }

  if (sizeof(DataFromObc2) == len) {
    memcpy(&rxData2, data, sizeof(DataFromObc2));
    moduleData.dataFromObc.commandNum = rxData2.commandNum;
    moduleData.dataFromObc.commandArg =
        (((uint32_t)(rxData2.arg1) << 16) & 0xFFFF0000) |
        (((uint32_t)(rxData2.arg2)) & 0x0000FFFF);
    ESP_LOGI("ESP-NOW", "Received command: %lu with arg1: %d and arg2: %d",
             moduleData.dataFromObc.commandNum, rxData2.arg1, rxData2.arg2);
    if (rxData2.commandNum == OBC_TEST_COMMAND) {
      obc_test_data_enabled = (rxData2.arg1 != 0);
      ESP_LOGI("NOW", "OBC test data %s",
               obc_test_data_enabled ? "enabled" : "disabled");
      return;
    }
    chandle_valve_cmd_angle((uint8_t)rxData2.commandNum, rxData2.arg1,
                            rxData2.arg2);
    return;
  }
}

void now_send_data_to_obc(void *arg) {
  while (1) {
    BoardData_t board_data_copy;
    DataToObc dataToObc;

    if (get_board_data(&board_data_copy, portMAX_DELAY) != ESP_OK) {
      ESP_LOGE("NOW", "Failed to take BoardData semaphore");
      continue;
    }

    dataToObc.waken_up = true;
    dataToObc.dump_valve_arm = board_data_copy.dump_valve_arm;
    dataToObc.dump_valve_cont = board_data_copy.dump_valve_cont;
    dataToObc.is_charging = board_data_copy.is_charging;
    dataToObc.temperature1 = board_data_copy.temperature[1];
    dataToObc.pressure1 = board_data_copy.pressure[2];
    dataToObc.pressure2 = board_data_copy.pressure[3];
    dataToObc.battery_voltage = board_data_copy.chargerData.vbat;
    dataToObc.battery_consumption =
        (fabsf(board_data_copy.chargerData.ibat) >
         fabsf(board_data_copy.chargerData.iin))
            ? fabsf(board_data_copy.chargerData.ibat)
            : fabsf(board_data_copy.chargerData.iin);
    dataToObc.charger_temperature = board_data_copy.chargerData.die_temp;
    dataToObc.valve1_state = valve1_state;
    dataToObc.valve2_state = valve2_state;
#ifdef SOL_ETH_CONFIG
    dataToObc.pressure1 = board_data_copy.pressure[0];
#endif

#ifdef SOL_N20_SERVO_ETH_CONFIG
    dataToObc.auto_vent_activated = is_auto_vent_active;
    dataToObc.auto_vent_triggered = is_triggered;
    float auto_vent_pressure_local = 0.0f;
    get_auto_vent_pressure(&auto_vent_pressure_local);
    dataToObc.auto_vent_pressure = (int32_t)(auto_vent_pressure_local * 1000);
#endif

    if (obc_test_data_enabled) {
      dataToObc.pressure1 = OBC_TEST_PRESSURE1;
      dataToObc.pressure2 = OBC_TEST_PRESSURE2;
      dataToObc.battery_voltage = OBC_TEST_BATTERY_VOLTAGE;
      dataToObc.bettery_consumption = OBC_TEST_CONSUMPTION;
      dataToObc.charger_temperature = OBC_TEST_CHARGER_TEMPERATURE;
      dataToObc.temperature1 = OBC_TEST_TEMPERATURE;
      dataToObc.is_charging = OBC_TEST_IS_CHARGING;
    }
    // ESP_LOGI("NOW", "Valve states: valve1_state=%u, valve2_state=%u",
    // valve1_state, valve2_state);

    if (now_send_data_log_enabled) {
      ESP_LOGI("NOW",
               "Sending data to OBC:\n"
               "  waken_up            = %d\n"
               "  dump_valve_arm      = %d\n"
               "  dump_valve_cont     = %d\n"
               "  is_charging         = %d\n"
               "  temperature1        = %d\n"
               "  pressure1           = %.2f\n"
               "  pressure2           = %.2f\n"
               "  battery_voltage     = %.2f\n"
               "  bettery_consumption = %.2f\n"
               "  charger_temperature = %.2f\n"
               "  valve1_state        = %u\n"
               "  valve2_state        = %u",
               dataToObc.waken_up, dataToObc.dump_valve_arm,
               dataToObc.dump_valve_cont, dataToObc.is_charging,
               dataToObc.temperature1, dataToObc.pressure1, dataToObc.pressure2,
               dataToObc.battery_voltage, dataToObc.bettery_consumption,
               dataToObc.charger_temperature,
               (unsigned int)dataToObc.valve1_state,
               (unsigned int)dataToObc.valve2_state);
#ifdef SOL_N20_SERVO_ETH_CONFIG
      ESP_LOGI("NOW",
               "\n  auto_vent_activated = %d\n"
               "  auto_vent_triggered = %d\n"
               "  auto_vent_pressure = %d\n",
               dataToObc.auto_vent_activated, dataToObc.auto_vent_triggered,
               dataToObc.auto_vent_pressure);
#endif
    }

    if (esp_now_send(addressObc, (uint8_t *)&dataToObc, sizeof(DataToObc)) !=
        ESP_OK) {
      ESP_LOGE("NOW", "Error sending data to OBC");
    }

    vTaskDelay(pdMS_TO_TICKS(moduleData.stateTimes[moduleData.obcState]));
  }
}

void createNowSendTask() {
  xTaskCreatePinnedToCore(now_send_data_to_obc, "now_send_task", 8192, NULL, 5,
                          NULL, 1);
}
