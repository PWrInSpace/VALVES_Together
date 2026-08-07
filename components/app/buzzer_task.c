#include "buzzer_task.h"

#include "buzzer.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#define TAG "BUZZER_TASK"

#define BUZZER_TASK_STACK_SIZE 4096
#define BUZZER_TASK_PRIORITY 2
#define BUZZER_QUEUE_LENGTH 8
#define TIME_BETWEEN_SOUNDS_MS 400

static QueueHandle_t buzzer_queue = NULL;
static TaskHandle_t buzzer_task_handle = NULL;
static volatile sound_id_t background_sound = SOUND_NONE;

static void buzzer_task(void *arg) {
  (void)arg;

  while (true) {
    sound_id_t sound;
    xQueueReceive(buzzer_queue, &sound, portMAX_DELAY);
    play_sound(sound);

    vTaskDelay(pdMS_TO_TICKS(TIME_BETWEEN_SOUNDS_MS));
  }
}

bool run_buzzer_task(void) {
  if (buzzer_task_handle != NULL) {
    return true;
  }

  buzzer_queue = xQueueCreate(BUZZER_QUEUE_LENGTH, sizeof(sound_id_t));
  if (buzzer_queue == NULL) {
    ESP_LOGE(TAG, "Failed to create buzzer queue");
    return false;
  }

  ESP_LOGI(TAG, "Starting buzzer task");
  if (xTaskCreate(buzzer_task, "buzzer_task", BUZZER_TASK_STACK_SIZE, NULL,
                  BUZZER_TASK_PRIORITY, &buzzer_task_handle) != pdPASS) {
    ESP_LOGE(TAG, "Failed to create buzzer_task");
    vQueueDelete(buzzer_queue);
    buzzer_queue = NULL;
    return false;
  }

  return true;
}

bool play_buzzer_sound(sound_id_t sound) {
  if (buzzer_queue == NULL) {
    return false;
  }
  return xQueueSend(buzzer_queue, &sound, 0) == pdTRUE;
}