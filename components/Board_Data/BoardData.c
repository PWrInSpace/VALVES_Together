#include "BoardData.h"
#include <esp_err.h>

volatile BoardData_t boardData;
SemaphoreHandle_t BoardDataSemaphore;
volatile ModuleData moduleData = {
    .dataFromObc = {0, 0},
    .inServiceMode = false,
    .dataToObc = {0, 0, 0, 0}
};
esp_err_t board_data_init(void) {
    // Initialize semaphore
    BoardDataSemaphore = xSemaphoreCreateMutex();
    if (BoardDataSemaphore == NULL) {
        return ESP_ERR_NO_MEM;
    }
    // Initialize board data (valves are initialized in valves_init())
    return ESP_OK;
}