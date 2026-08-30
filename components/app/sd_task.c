#include "sd_task.h"
#include "BoardData.h"
#include "auto_vent_task.h"
#include "buzzer.h"
#include "buzzer_task.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "mcu_spi_config.h"
#include "sdcard.h"
#include <dirent.h>

#define TAG "SD_TASK"
#define BUFFER_SAMPLES 250
static sd_card_t sd_card;
TaskHandle_t sd_task = NULL;
TaskHandle_t sd_data_task = NULL;

BoardData_t *buffer_A;
BoardData_t *buffer_B;
BoardData_t *current_buffer = NULL;

uint32_t buffer_readc_index = 0;

SemaphoreHandle_t mutex_A = NULL;
SemaphoreHandle_t mutex_B = NULL;
SemaphoreHandle_t current_mutex = NULL;

SemaphoreHandle_t buffer_A_ready = NULL;
SemaphoreHandle_t buffer_B_ready = NULL;
SemaphoreHandle_t current_sync = NULL;

static void get_next_log_filename(char *out_name, size_t max_len) {
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

  buffer_A = (BoardData_t *)heap_caps_malloc(
      BUFFER_SAMPLES * sizeof(BoardData_t), MALLOC_CAP_DMA);
  buffer_B = (BoardData_t *)heap_caps_malloc(
      BUFFER_SAMPLES * sizeof(BoardData_t), MALLOC_CAP_DMA);

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

  sd_card_config_t config = {.mount_point = MOUNT_POINT, .cd_pin = -1};

  if (!SD_init(&sd_card, &config)) {
    ESP_LOGE("SD_TASK", "Failed to initialize SD card");
    return ESP_FAIL;
  } else {
    ESP_LOGI("SD_TASK", "SD card initialized successfully");
  }

  if (!run_sd_task()) {
    ESP_LOGE("SD_TASK", "Failed to start readc SD task");
    return ESP_FAIL;
  }

  if (!run_update_data_task()) {
    ESP_LOGE("SD_TASK", "Failed to start update data task");
    return ESP_FAIL;
  }

  return ESP_OK;
}

static bool save_text(const char *path, BoardData_t *data) {
  FILE *f = fopen(path, "a"); // append text
  if (!f) {
    ESP_LOGE("SDCARD", "Failed to open %s for writing", path);
    return false;
  }

  for (uint32_t i = 0; i < BUFFER_SAMPLES; i++) {
#ifdef SOL_N20_SERVO_ETH_CONFIG
    fprintf(f,
            "%llu,%f,%f,%f,%f,%f,%f,%f,"
            "%d,%d,%d,%d,%d,"
            "%f,%f,%f,%f,%f,%f,%f,%f,"
            "%d,%d,%d,"
            "%d,%d,%d,%ld\n",
            (unsigned long long)data[i].power_time, data[i].temperature[0],
            data[i].temperature[1], data[i].temperature[2], data[i].pressure[1],
            data[i].pressure[2], data[i].pressure[3], data[i].termistor,
            data[i].dump_valve_cont, data[i].dump_valve_arm, valve1_state,
            valve2_state, data[i].is_charging, data[i].chargerData.vbat,
            data[i].chargerData.vin, data[i].chargerData.vin_supply,
            data[i].chargerData.vext_supply, data[i].chargerData.ibat,
            data[i].chargerData.iin, data[i].chargerData.die_temp,
            data[i].chargerData.vout, data[i].chargerData.charger_status,
            data[i].chargerData.charger_state,
            data[i].chargerData.system_status, moduleData.obcState,
            data[i].auto_vent_activated, data[i].auto_vent_triggered,
            (long)data[i].auto_vent_pressure);
#else
    fprintf(f,
            "%llu,%f,%f,%f,%f,%f,%f,%f,"
            "%d,%d,%d,%d,%d,"
            "%f,%f,%f,%f,%f,%f,%f,%f,"
            "%d,%d,%d,"
            "%d\n",
            (unsigned long long)data[i].power_time, data[i].temperature[0],
            data[i].temperature[1], data[i].temperature[2], data[i].pressure[1],
            data[i].pressure[2], data[i].pressure[3], data[i].termistor,
            data[i].dump_valve_cont, data[i].dump_valve_arm, valve1_state,
            valve2_state, data[i].is_charging, data[i].chargerData.vbat,
            data[i].chargerData.vin, data[i].chargerData.vin_supply,
            data[i].chargerData.vext_supply, data[i].chargerData.ibat,
            data[i].chargerData.iin, data[i].chargerData.die_temp,
            data[i].chargerData.vout, data[i].chargerData.charger_status,
            data[i].chargerData.charger_state,
            data[i].chargerData.system_status, moduleData.obcState);
#endif
  }

  fclose(f);
  return true;
}

static bool add_header(const char *path) {
  FILE *f = fopen(path, "a");
  if (!f) {
    ESP_LOGE("SDCARD", "Failed to open %s for writing", path);
    return false;
  }
  fprintf(f, "Log file for configuration: %s\n", CONFIG_NAME);
#ifdef SOL_N20_SERVO_ETH_CONFIG
  fprintf(f, "PowerTime,Temp1,Temp2,Temp3,Press1,Press2,Press3,Termistor,"
             "DumpValveCont,DumpValveArm,Valve1State,Valve2State,IsCharging,"
             "Vbat,Vin,VinSupply,VextSupply,Ibat,Iin,DieTemp,Vout,"
             "ChargerStatus,ChargerState,SystemStatus,"
             "ObcState,AutoVentActivated,AutoVentTriggered,AutoVentPressure\n");
#else
  fprintf(f, "PowerTime,Temp1,Temp2,Temp3,Press1,Press2,Press3,Termistor,"
             "DumpValveCont,DumpValveArm,Valve1State,Valve2State,IsCharging,"
             "Vbat,Vin,VinSupply,VextSupply,Ibat,Iin,DieTemp,Vout,"
             "ChargerStatus,ChargerState,SystemStatus,"
             "ObcState\n");
#endif

  fclose(f);
  ESP_LOGI("SDCARD", "Header added to %s", path);
  return true;
}

static void save_buffer(const char *path, BoardData_t *data) {
  if (sd_card.mounted) {
    if (!save_text(path, data)) {
      ESP_LOGE(TAG, "Failed to save data to SD card");
    }
  } else {
    ESP_LOGW(TAG, "SD card is not mounted, skipping save operation");
  }
}
void update_data_task(void *arg) {
  int counter = 0;
  while (1) {
    BoardData_t boardDataCopy;

    if (get_board_data(&boardDataCopy, portMAX_DELAY) != ESP_OK) {
      ESP_LOGE(TAG, "Failed to get board data");
      continue;
    }

#ifdef SOL_N20_SERVO_ETH_CONFIG // TODO dziala ale trzeba to kiedys zrobic
                                // lepiej, autovent task nie updatuje tych
                                // danych
    boardDataCopy.auto_vent_activated = is_auto_vent_active;
    boardDataCopy.auto_vent_triggered = is_triggered;
    float avp = 0.0f;
    get_auto_vent_pressure(&avp);
    boardDataCopy.auto_vent_pressure = (int32_t)(avp * 1000);
#endif

    if (xSemaphoreTake(current_mutex, portMAX_DELAY) == pdTRUE) {
      boardDataCopy.power_time = power_time();
      current_buffer[counter] = boardDataCopy;
      counter++;
      xSemaphoreGive(current_mutex);
    }

    if (counter >= BUFFER_SAMPLES) {
      xSemaphoreGive(current_sync);

      if (current_buffer == buffer_A) {
        current_buffer = buffer_B;
        current_mutex = mutex_B;
        current_sync = buffer_B_ready;
      } else {
        current_buffer = buffer_A;
        current_mutex = mutex_A;
        current_sync = buffer_A_ready;
      }
      counter = 0;
    }

    vTaskDelay(pdMS_TO_TICKS(moduleData.sdStateTimes[moduleData.obcState]));
    // vTaskDelay(pdMS_TO_TICKS(10));
  }
}

static void save_data_task(void *arg) {
  char file_path[64];
  get_next_log_filename(file_path, sizeof(file_path));
  ESP_LOGI(TAG, "Saving to %s", file_path);
  add_header(file_path);

  ESP_LOGI(TAG, "Starting SD card save task");

  while (1) {
    if (xSemaphoreTake(buffer_A_ready, portMAX_DELAY) == pdTRUE) {
      if (xSemaphoreTake(mutex_A, portMAX_DELAY) == pdTRUE) {
        save_buffer(file_path, buffer_A);
        xSemaphoreGive(mutex_A);
      }
    }
    if (xSemaphoreTake(buffer_B_ready, portMAX_DELAY) == pdTRUE) {
      if (xSemaphoreTake(mutex_B, portMAX_DELAY) == pdTRUE) {
        save_buffer(file_path, buffer_B);
        xSemaphoreGive(mutex_B);
      }
    }
  }
}

bool run_update_data_task() {
  ESP_LOGI("SD_TASK", "Starting SD card update data task");
  BaseType_t result = xTaskCreate(update_data_task, "update_data_task", 8192,
                                  NULL, 5, &sd_data_task);

  if (result != pdPASS) {
    ESP_LOGE("SD_TASK", "Failed to create save_data_task");
    return false;
  }

  return true;
}

bool run_sd_task() {
  ESP_LOGI("SD_TASK", "Starting SD card readc task");
  BaseType_t result =
      xTaskCreate(save_data_task, "save_data_task", 8192, NULL, 5, &sd_task);

  if (result != pdPASS) {
    ESP_LOGE("SD_TASK", "Failed to create save_data_task");
    return false;
  }

  return true;
}