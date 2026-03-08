#include "BoardData.h"
#include <esp_err.h>
#include <string.h>
#include "valve_board_config.h"
#include "esp_timer.h"

BoardData_t boardData;
SemaphoreHandle_t BoardDataSemaphore;
volatile ModuleData moduleData = {
    .dataFromObc = {0, 0},
    .dataToObc = {true, 0, 0, 0, 0, 0, 0, 0, 0.0f},
    .obcState = 0,
    .stateTimes = {0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
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

    uint16_t stateTimes[16] = {
        INIT_PERIOD,
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

    memcpy((uint8_t *)moduleData.stateTimes, (uint8_t *)stateTimes, sizeof(stateTimes));

    return ESP_OK;
}

uint64_t power_time()
{
    return (esp_timer_get_time() - start_time_us);
}

