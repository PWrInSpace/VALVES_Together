#include "valve_board_config.h"
#ifdef SOL_N20_SERVO_ETH_CONFIG

#include "BoardData.h"
#include "auto_vent_task.h"
#include "commands.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "valves_control.h"

SemaphoreHandle_t auto_vent_pressure_mutex = NULL;

volatile bool is_auto_vent_active = false;
volatile bool is_triggered = false;
float auto_vent_pressure = 0;
TaskHandle_t auto_vent_task_handle = NULL;
#define TAG "AUTO_VENT_TASK"

static float get_pressure_from_board() {
  float pressures[4];
  if (get_boardData_pressures(pressures, portMAX_DELAY) != ESP_OK)
    return 0.0f;

  return pressures[1]; // left pressure channel on N20 vent
                       // board with n2o pressure sensor
}

static float get_median_pressure(int samples) {
  // n samples at 50 ms each
  if (samples <= 0) {
    return 0.0f;
  }
  if (samples > 32) {
    samples = 32;
  }

  float buf[32];
  for (int i = 0; i < samples; i++) {
    buf[i] = get_pressure_from_board();
    vTaskDelay(pdMS_TO_TICKS(50));
  }

  for (int i = 1; i < samples; i++) {
    float key = buf[i];
    int j = i - 1;
    while (j >= 0 && buf[j] > key) {
      buf[j + 1] = buf[j];
      j--;
    }
    buf[j + 1] = key;
  }

  if (samples % 2 == 0) {
    return (buf[samples / 2 - 1] + buf[samples / 2]) * 0.5f;
  }
  return buf[samples / 2];
}

bool set_auto_vent_pressure(float pressure) {
  if (auto_vent_pressure_mutex == NULL) {
    return false;
  }
  if (xSemaphoreTake(auto_vent_pressure_mutex, portMAX_DELAY) == pdTRUE) {
    auto_vent_pressure = pressure;
    xSemaphoreGive(auto_vent_pressure_mutex);
    return true;
  }
  return false;
}

bool get_auto_vent_pressure(float *pressure) {
  if (auto_vent_pressure_mutex == NULL) {
    *pressure = 0.0f;
    return false;
  }
  if (xSemaphoreTake(auto_vent_pressure_mutex, portMAX_DELAY) == pdTRUE) {
    *pressure = auto_vent_pressure;
    xSemaphoreGive(auto_vent_pressure_mutex);
    return true;
  }
  return false;
}

bool set_auto_vent_on(float pressure) {
  set_auto_vent_pressure(pressure);
  is_auto_vent_active = true;
  return true;
}

bool set_auto_vent_off() {
  is_auto_vent_active = false;
  is_triggered = false;
  set_auto_vent_pressure(0.0f);
  return true;
}

void auto_vent_task(void *arg) {
  TickType_t triggered_at = 0;
  while (1) {
    float median_pressure = 0.0f;
    float auto_vent_pressure_local = 0.0f;
    if (is_auto_vent_active) {
      median_pressure = get_median_pressure(20); // ~1 s, 20 samples
      get_auto_vent_pressure(&auto_vent_pressure_local);
      if (median_pressure > auto_vent_pressure_local && is_auto_vent_active) {
        handle_valve_cmd(N20_SOL_OPEN, AUTO_VENT_OPEN_TIME_MS);
        is_triggered = true;
        triggered_at = xTaskGetTickCount();
        vTaskDelay(pdMS_TO_TICKS(AUTO_VENT_DEBOUNCE_TIME_MS));
      } else if (is_triggered &&
                 (xTaskGetTickCount() - triggered_at) >=
                     pdMS_TO_TICKS(AUTO_VENT_TRIGGERED_STATUS_MS)) {
        is_triggered = false;
      }
    } else {
      vTaskDelay(pdMS_TO_TICKS(5000));
    }
  }
}

bool run_auto_vent_task() {
  if (auto_vent_pressure_mutex == NULL) {
    auto_vent_pressure_mutex = xSemaphoreCreateMutex();
    if (auto_vent_pressure_mutex == NULL) {
      ESP_LOGE(TAG, "Failed to create auto_vent_pressure_mutex");
      return false;
    }
  }
  if (xTaskCreate(auto_vent_task, "auto_vent_task", 8192, NULL, 5,
                  &auto_vent_task_handle) != pdPASS) {
    ESP_LOGE(TAG, "Failed to create auto_vent_task");
    return false;
  }
  return true;
}

#endif