#include "BoardData.h"
#include "esp_timer.h"
#include "valve_board_config.h"
#include <esp_err.h>
#include <string.h>

#define TAG "BoardData"

BoardData_t boardData;
SemaphoreHandle_t BoardDataSemaphore;
volatile ModuleData moduleData = {
    .dataFromObc = {0, 0},
    .dataToObc = {true, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    .obcState = 0,
    .stateTimes = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    .sdStateTimes = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

volatile uint8_t valve1_state = 0;
volatile uint8_t valve2_state = 0;
uint64_t start_time_us;

esp_err_t board_data_init(void) {
  start_time_us = esp_timer_get_time();
  BoardDataSemaphore = xSemaphoreCreateMutex();
  if (BoardDataSemaphore == NULL) {
    return ESP_ERR_NO_MEM;
  }

  uint16_t stateTimes[16] = {INIT_PERIOD,
                             IDLE_PERIOD,
                             RECOVERY_ARM_PERIOD,
                             FUELING_PERIOD,
                             PRESSURIZING_PERIOD,
                             ARMED_TO_LAUNCH_PERIOD,
                             RDY_TO_LAUNCH_PERIOD,
                             COUNTDOWN_PERIOD,
                             LIFT_OFF_PERIOD,
                             BURN_PERIOD,
                             FLIGHT_PERIOD,
                             FIRST_STAGE_REC_PERIOD,
                             SECOND_STAGE_REC_PERIOD,
                             ON_GROUND_PERIOD,
                             HOLD_PERIOD,
                             ABORT_PERIOD};

  memcpy((uint8_t *)moduleData.stateTimes, (uint8_t *)stateTimes,
         sizeof(stateTimes));

  uint16_t sdStateTimes[16] = {SD_INIT_PERIOD,
                               SD_IDLE_PERIOD,
                               SD_RECOVERY_ARM_PERIOD,
                               SD_FUELING_PERIOD,
                               SD_PRESSURIZING_PERIOD,
                               SD_ARMED_TO_LAUNCH_PERIOD,
                               SD_RDY_TO_LAUNCH_PERIOD,
                               SD_COUNTDOWN_PERIOD,
                               SD_LIFT_OFF_PERIOD,
                               SD_BURN_PERIOD,
                               SD_FLIGHT_PERIOD,
                               SD_FIRST_STAGE_REC_PERIOD,
                               SD_SECOND_STAGE_REC_PERIOD,
                               SD_ON_GROUND_PERIOD,
                               SD_HOLD_PERIOD,
                               SD_ABORT_PERIOD};

  memcpy((uint8_t *)moduleData.sdStateTimes, (uint8_t *)sdStateTimes,
         sizeof(sdStateTimes));

  return ESP_OK;
}

uint64_t power_time() { return (esp_timer_get_time() - start_time_us); }

void print_board_data(void) {
  BoardData_t bd_data;
  get_board_data(&bd_data, portMAX_DELAY);

  ESP_LOGI(TAG, "--------------------------------");
  ESP_LOGI(TAG, "Power time: %llu", bd_data.power_time);
  ESP_LOGI(TAG, "Temperature 1: %f", boardData.temperature[0]);
  ESP_LOGI(TAG, "Temperature 2: %f", boardData.temperature[1]);
  ESP_LOGI(TAG, "Temperature 3: %f", boardData.temperature[2]);
  ESP_LOGI(TAG, "Pressure L = %f S = %f P1 = %f P2 = %f", bd_data.pressure[1],
           bd_data.pressure[2], bd_data.pressure[3], bd_data.pressure[0]);
  ESP_LOGI(TAG, "valve1_state = %d valve2_state = %d", valve1_state,
           valve2_state);
  ESP_LOGI(TAG, "Is charging: %s", bd_data.is_charging ? "true" : "false");
  ESP_LOGI(TAG, "VINT = %f", bd_data.chargerData.vin_supply);
  ESP_LOGI(TAG, "VEXT = %f", bd_data.chargerData.vext_supply);
  ESP_LOGI(TAG, "Charge temperature = %f C", bd_data.chargerData.die_temp);
  ESP_LOGI(TAG, "Current - ibat = %f A", bd_data.chargerData.ibat);
  ESP_LOGI(TAG, "Current - iin = %f A", bd_data.chargerData.iin);
  ESP_LOGI(TAG, "--------------------------------");
}

esp_err_t get_board_data(BoardData_t *data, uint32_t mutexTimeout) {
  if (xSemaphoreTake(BoardDataSemaphore, mutexTimeout) != pdTRUE)
    return ESP_ERR_TIMEOUT;

  *data = boardData;
  xSemaphoreGive(BoardDataSemaphore);

  return ESP_OK;
}

esp_err_t set_board_data(BoardData_t data, uint32_t mutexTimeout) {
  if (xSemaphoreTake(BoardDataSemaphore, mutexTimeout) != pdTRUE)
    return ESP_ERR_TIMEOUT;

  boardData = data;
  xSemaphoreGive(BoardDataSemaphore);

  return ESP_OK;
}

esp_err_t get_boardData_charger_data(ltc4162_charger_data_t *data,
                                     uint32_t mutexTimeout) {
  if (xSemaphoreTake(BoardDataSemaphore, mutexTimeout) != pdTRUE)
    return ESP_ERR_TIMEOUT;

  *data = boardData.chargerData;
  xSemaphoreGive(BoardDataSemaphore);

  return ESP_OK;
}

esp_err_t set_boardData_charger_data(ltc4162_charger_data_t data,
                                     uint32_t mutexTimeout) {
  if (xSemaphoreTake(BoardDataSemaphore, mutexTimeout) != pdTRUE)
    return ESP_ERR_TIMEOUT;

  boardData.chargerData = data;
  xSemaphoreGive(BoardDataSemaphore);

  return ESP_OK;
}

esp_err_t set_boardData_pressures(float pressures[4], uint32_t mutexTimeout) {
  if (xSemaphoreTake(BoardDataSemaphore, mutexTimeout) != pdTRUE)
    return ESP_ERR_TIMEOUT;

  for (int i = 0; i < 4; i++)
    boardData.pressure[i] = pressures[i];
  xSemaphoreGive(BoardDataSemaphore);

  return ESP_OK;
}

esp_err_t get_boardData_pressures(float pressures[4], uint32_t mutexTimeout) {
  if (xSemaphoreTake(BoardDataSemaphore, mutexTimeout) != pdTRUE)
    return ESP_ERR_TIMEOUT;

  for (int i = 0; i < 4; i++)
    pressures[i] = boardData.pressure[i];
  xSemaphoreGive(BoardDataSemaphore);

  return ESP_OK;
}

esp_err_t set_boardData_temperatures(float temperatures[3],
                                     uint32_t mutexTimeout) {
  if (xSemaphoreTake(BoardDataSemaphore, mutexTimeout) != pdTRUE)
    return ESP_ERR_TIMEOUT;

  for (int i = 0; i < 3; i++)
    boardData.temperature[i] = temperatures[i];
  xSemaphoreGive(BoardDataSemaphore);

  return ESP_OK;
}

esp_err_t get_boardData_temperatures(float temperatures[3],
                                     uint32_t mutexTimeout) {
  if (xSemaphoreTake(BoardDataSemaphore, mutexTimeout) != pdTRUE)
    return ESP_ERR_TIMEOUT;

  for (int i = 0; i < 3; i++)
    temperatures[i] = boardData.temperature[i];
  xSemaphoreGive(BoardDataSemaphore);

  return ESP_OK;
}

esp_err_t get_boardData_power_time(uint64_t *power_time,
                                   uint32_t mutexTimeout) {
  if (xSemaphoreTake(BoardDataSemaphore, mutexTimeout) != pdTRUE)
    return ESP_ERR_TIMEOUT;

  *power_time = boardData.power_time;
  xSemaphoreGive(BoardDataSemaphore);

  return ESP_OK;
}

esp_err_t set_boardData_power_time(uint64_t power_time, uint32_t mutexTimeout) {
  if (xSemaphoreTake(BoardDataSemaphore, mutexTimeout) != pdTRUE)
    return ESP_ERR_TIMEOUT;

  boardData.power_time = power_time;
  xSemaphoreGive(BoardDataSemaphore);

  return ESP_OK;
}