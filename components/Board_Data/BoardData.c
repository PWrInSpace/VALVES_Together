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
