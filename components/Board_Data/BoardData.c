#include "BoardData.h"
#include <esp_err.h>
#include <string.h>

volatile BoardData_t boardData;
SemaphoreHandle_t BoardDataSemaphore;
volatile ModuleData moduleData = {
    .dataFromObc = {0, 0},
    .dataToObc = {true, 0, 0, 0, 0, 0, 0, 0, 0.0f},
    .obcState = 0,
    .stateTimes = {0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};
esp_err_t board_data_init(void) {
    // Initialize semaphore``````````````````

    BoardDataSemaphore = xSemaphoreCreateMutex();
    if (BoardDataSemaphore == NULL) {
        return ESP_ERR_NO_MEM;
    }

    uint16_t stateTimes[16] = {
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

    // Initialize board data (valves are initialized in valves_init())
    return ESP_OK;
}

void set_valve1_state(uint8_t state) {
    if (xSemaphoreTake(BoardDataSemaphore, portMAX_DELAY) == pdTRUE) {
        moduleData.dataToObc.valve1_state = state;
        xSemaphoreGive(BoardDataSemaphore);
    }
}

void set_valve2_state(uint8_t state) {
    if (xSemaphoreTake(BoardDataSemaphore, portMAX_DELAY) == pdTRUE) {
        moduleData.dataToObc.valve2_state = state;
        xSemaphoreGive(BoardDataSemaphore);
    }
}