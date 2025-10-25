#include "sd_task.h"
#include "sdcard.h"
#include "esp_log.h"
#include "mcu_spi_config.h" //mutex_spi
#include <dirent.h>
#include "BoardData.h"
#include "esp_timer.h"

#define TAG "SD_TASK"
#define BUFFER_SAMPLES 250
static sd_card_t sd_card;
TaskHandle_t sd_task = NULL;
TaskHandle_t sd_data_task = NULL;

BoardData_t* buffer_A;
BoardData_t* buffer_B;
BoardData_t* current_buffer = NULL;

uint32_t buffer_readc_index = 0;

SemaphoreHandle_t mutex_A = NULL;
SemaphoreHandle_t mutex_B = NULL;
SemaphoreHandle_t current_mutex = NULL;

SemaphoreHandle_t buffer_A_ready = NULL;
SemaphoreHandle_t buffer_B_ready = NULL;
SemaphoreHandle_t current_sync = NULL;

void get_next_log_filename(char *out_name, size_t max_len)
{
    DIR *dir = opendir(MOUNT_POINT);
    struct dirent *entry;
    int max_index = 0;

    if (dir == NULL) {
        printf("Failed to open dir\n");
        snprintf(out_name, max_len, "%s/log_001.csv", MOUNT_POINT);
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            int index;
            if (sscanf(entry->d_name, "LOG_%03d.csv", &index) == 1) {
                if (index > max_index) {
                    max_index = index;
                }
            }
        }
    }
    closedir(dir);

    snprintf(out_name, max_len, "%s/log_%03d.csv", MOUNT_POINT, max_index + 1);
}

esp_err_t sd_task_init(void) {

    buffer_A = (BoardData_t*)heap_caps_malloc(BUFFER_SAMPLES * sizeof(BoardData_t), MALLOC_CAP_DMA);
    buffer_B = (BoardData_t*)heap_caps_malloc(BUFFER_SAMPLES * sizeof(BoardData_t), MALLOC_CAP_DMA);

    if (buffer_A == NULL || buffer_B == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for SD buffers");
        return ESP_FAIL;
    }


    mutex_A = xSemaphoreCreateMutex();
    mutex_B = xSemaphoreCreateMutex();

    buffer_A_ready = xSemaphoreCreateBinary();
    buffer_B_ready = xSemaphoreCreateBinary();

    current_buffer = buffer_A;
    current_mutex = mutex_A;
    current_sync = buffer_A_ready; 

    

    sd_card_config_t config = {
        .spi_host = SDSPI_DEFAULT_HOST,
        .mount_point = MOUNT_POINT,
        .cs_pin = 5,
        .cd_pin = -1
    };

    if (!SD_init(&sd_card, &config)) {
        ESP_LOGE("SD_TASK", "Failed to initialize SD card");
        return ESP_FAIL;
    } else {
        ESP_LOGI("SD_TASK", "SD card initialized successfully");
    }

    if(!run_sd_task())
    {
        ESP_LOGE("SD_TASK", "Failed to start readc SD task");
        return ESP_FAIL;
    }

    if(!run_update_data_task())
    {
        ESP_LOGE("SD_TASK", "Failed to start update data task");
        return ESP_FAIL;
    }

    return ESP_OK;

}

bool save_text(const char* path, BoardData_t* data) {
    FILE* f = fopen(path, "a");  // append text
    if (!f) {
        ESP_LOGE("SDCARD", "Failed to open %s for writing", path);
        return false;
    }

    for (uint32_t i = 0; i < BUFFER_SAMPLES; i++) {
        fprintf(f, "%u,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%u,%u\n",
                data[i].time_ms,
                data[i].temperature[0],
                data[i].temperature[1],
                data[i].temperature[2],
                data[i].pressure[0],
                data[i].pressure[1],
                data[i].battery_voltage,
                data[i].valve_state[0],
                data[i].valve_state[1]);
    }

    fclose(f);
    return true;
}

bool add_header(const char* path) {
    FILE* f = fopen(path, "a");
    if (!f) {
        ESP_LOGE("SDCARD", "Failed to open %s for writing", path);
        return false;
    }
    #ifdef SOL_N2O_N2_CONFIG
    fprintf(f, "Time [ms], Temperature1 [C], Temperature2 [C], Temperature3 [C], Pressure1 [kPa], Pressure2 [kPa], Battery Voltage [V], N2O_SOL_STATE, N2_SOL_STATE\n");
    #elif defined(SOL_ETH_CONFIG)
    fprintf(f, "Time [ms], Temperature1 [C], Temperature2 [C], Temperature3 [C], Pressure1 [kPa], Pressure2 [kPa], Battery Voltage [V], ETH_SOL_STATE, ignore\n");
    #elif defined(SERVO_N20_CONFIG)
    fprintf(f, "Time [ms], Temperature1 [C], Temperature2 [C], Temperature3 [C], Pressure1 [kPa], Pressure2 [kPa], Battery Voltage [V], N2O_VALVE_STATE, ignore\n");
    #elif defined(SERVO_ETH_N2_CONFIG)
    fprintf(f, "Time [ms], Temperature1 [C], Temperature2 [C], Temperature3 [C], Pressure1 [kPa], Pressure2 [kPa], Battery Voltage [V], ETH_VALVE_STATE, N2_VALVE_STATE\n");
    #else
    fprintf(f, "Time [ms], Temperature1 [C], Temperature2 [C], Temperature3 [C], Pressure1 [kPa], Pressure2 [kPa], Battery Voltage [V], Valve1_State, Valve2_State\n");
    #endif

    fclose(f);
    ESP_LOGI("SDCARD", "Header added to %s", path);
    return true;
}

void save_buffer(const char* path, BoardData_t* data) {
    if (sd_card.mounted) {
        if (!save_text(path, data)) {
            ESP_LOGE(TAG, "Failed to save data to SD card");
        }
    } else {
        ESP_LOGW(TAG, "SD card is not mounted, skipping save operation");
    }
}
void update_data_task(void *arg)
{
    int counter = 0;
    while (1)
    {
        BoardData_t boardDataCopy;
        if (xSemaphoreTake(BoardDataSemaphore, portMAX_DELAY) == pdTRUE) {
            memcpy(&boardDataCopy, (const void*)&boardData, sizeof(BoardData_t));
            xSemaphoreGive(BoardDataSemaphore);
        }

        if (xSemaphoreTake(current_mutex, portMAX_DELAY) == pdTRUE) {
            current_buffer[counter] = boardDataCopy;
            counter++;
            if (counter >= BUFFER_SAMPLES)
            {
                SemaphoreHandle_t save_mutex = current_mutex;
                BoardData_t* save_buffer = current_buffer;

                SemaphoreHandle_t other_mutex = (current_mutex == mutex_A) ? mutex_B : mutex_A;
                BoardData_t* other_buffer = (current_buffer == buffer_A) ? buffer_B : buffer_A;

                if (xSemaphoreTake(other_mutex, portMAX_DELAY) == pdTRUE) {
                    current_buffer = other_buffer;
                    current_mutex = other_mutex;
                    counter = 0;

                    if (save_buffer == buffer_A) {
                        xSemaphoreGive(buffer_A_ready);
                    } else {
                        xSemaphoreGive(buffer_B_ready);
                    }
                    xSemaphoreGive(save_mutex);
                } else {
                    ESP_LOGW(TAG, "Nie można wziąć mutexu drugiego bufora, kontynuuję z tym samym buforem");
                    xSemaphoreGive(save_mutex);
                }
            } else {
                xSemaphoreGive(current_mutex);
            }
        } // end if take current_mutex

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void save_data_task(void *arg)
{
    char file_path[64];
    get_next_log_filename(file_path, sizeof(file_path));
    ESP_LOGI(TAG, "Saving to %s", file_path);
    add_header(file_path);
    
    ESP_LOGI(TAG, "Starting SD card save task");


    while (1)
    {
        if(xSemaphoreTake(buffer_A_ready, portMAX_DELAY) == pdTRUE) {
            if(xSemaphoreTake(mutex_A, portMAX_DELAY) == pdTRUE) {
                save_buffer(file_path, buffer_A);
                xSemaphoreGive(mutex_A);
            }
        }
        if(xSemaphoreTake(buffer_B_ready, portMAX_DELAY) == pdTRUE) {
            if(xSemaphoreTake(mutex_B, portMAX_DELAY) == pdTRUE) {
                save_buffer(file_path, buffer_B);
                xSemaphoreGive(mutex_B);
            }
        }
            
    }
}

bool run_update_data_task()
{
    ESP_LOGI("SD_TASK", "Starting SD card update data task");
    BaseType_t result = xTaskCreate(
        update_data_task,
        "update_data_task",
        8192,
        NULL,
        5,
        &sd_data_task
    );

    if (result != pdPASS) {
        ESP_LOGE("SD_TASK", "Failed to create save_data_task");
        return false;
    }

    return true;
}

bool run_sd_task()
{
    ESP_LOGI("SD_TASK", "Starting SD card readc task");
    BaseType_t result = xTaskCreate(
        save_data_task,
        "save_data_task",
        8192,
        NULL,
        5,
        &sd_task
    );

    if (result != pdPASS) {
        ESP_LOGE("SD_TASK", "Failed to create save_data_task");
        return false;
    }

    return true;
}