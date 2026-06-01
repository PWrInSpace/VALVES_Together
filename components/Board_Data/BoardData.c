#include "BoardData.h"
#include "esp_timer.h"
#include "valve_board_config.h"
#include <esp_err.h>
#include <string.h>

BoardData_t boardData;
SemaphoreHandle_t BoardDataSemaphore;
volatile ModuleData moduleData = {
    .dataFromObc = {0, 0},
    .dataToObc = {true, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    .obcState = 0,
    .stateTimes = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
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

  return ESP_OK;
}

uint64_t power_time() { return (esp_timer_get_time() - start_time_us); }

esp_err_t get_board_data(BoardData_t *data, uint32_t mutexTimeout) {
  if (xSemaphoreTake(BoardDataSemaphore, mutexTimeout) != pdTRUE) return ESP_ERR_TIMEOUT;
  
  *data = boardData;
  xSemaphoreGive(BoardDataSemaphore);

  return ESP_OK;
}

esp_err_t get_boardData_charger_data(ChargerData_t *data, uint32_t mutexTimeout) {
  if (xSemaphoreTake(BoardDataSemaphore, mutexTimeout) != pdTRUE) return ESP_ERR_TIMEOUT;
  
  *data = boardData.chargerData;
  xSemaphoreGive(BoardDataSemaphore);

  return ESP_OK;
}

esp_err_t set_boardData_charger_data(ChargerData_t data, uint32_t mutexTimeout) {
  if (xSemaphoreTake(BoardDataSemaphore, mutexTimeout) != pdTRUE) return ESP_ERR_TIMEOUT;
  
  boardData.chargerData = data;
  xSemaphoreGive(BoardDataSemaphore);

  return ESP_OK;
}

esp_err_t set_boardData_pressures(float pressures[4], uint32_t mutexTimeout) {
  if (xSemaphoreTake(BoardDataSemaphore, mutexTimeout) != pdTRUE) return ESP_ERR_TIMEOUT;
  
  for (int i = 0; i < 4; i++) boardData.pressure[i] = pressures[i];
  xSemaphoreGive(BoardDataSemaphore);

  return ESP_OK;
}

esp_err_t get_boardData_pressures(float *pressures[4], uint32_t mutexTimeout) {
  if (xSemaphoreTake(BoardDataSemaphore, mutexTimeout) != pdTRUE) return ESP_ERR_TIMEOUT;
  
  *pressures = boardData.pressure;
  xSemaphoreGive(BoardDataSemaphore);

  return ESP_OK;
}

esp_err_t set_boardData_temperatures(float temperatures[3], uint32_t mutexTimeout) {
  if (xSemaphoreTake(BoardDataSemaphore, mutexTimeout) != pdTRUE) return ESP_ERR_TIMEOUT;
  
  for (int i = 0; i < 3; i++) boardData.temperature[i] = temperatures[i];
  xSemaphoreGive(BoardDataSemaphore);

  return ESP_OK;
}

esp_err_t get_boardData_temperatures(float *temperatures[3], uint32_t mutexTimeout) {
  if (xSemaphoreTake(BoardDataSemaphore, mutexTimeout) != pdTRUE) return ESP_ERR_TIMEOUT;
  
  *temperatures = boardData.temperature;
  xSemaphoreGive(BoardDataSemaphore);

  return ESP_OK;
}

esp_err_t get_boardData_power_time(uint64_t *power_time, uint32_t mutexTimeout) {
  if (xSemaphoreTake(BoardDataSemaphore, mutexTimeout) != pdTRUE) return ESP_ERR_TIMEOUT;
  
  *power_time = boardData.power_time;
  xSemaphoreGive(BoardDataSemaphore);

  return ESP_OK;
}

esp_err_t set_boardData_power_time(uint64_t power_time, uint32_t mutexTimeout) {
  if (xSemaphoreTake(BoardDataSemaphore, mutexTimeout) != pdTRUE) return ESP_ERR_TIMEOUT;
  
  boardData.power_time = power_time;
  xSemaphoreGive(BoardDataSemaphore);

  return ESP_OK;
}