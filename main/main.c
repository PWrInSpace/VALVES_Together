#include "app_task.h"
#include "board_config.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "setup_task.h"
#include "timers_config.h"
#include "buzzer.h"
#include "esp_random.h"

#define TAG "APP"

extern board_config_t config;

void app_main(void)
{

    ESP_LOGI(TAG, "%s VALVES_Together board starting", config.board_name);

    if (setup_task_init() != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize setup task");
        return;
    }

    int i = esp_random() % 6;

    switch (i)
    {
    case 0:
        ode_to_joy();
        break;
    case 1:
        imperial_march();
        break;
    case 2:
        harry_potter_theme();   
        break;
    case 3:
        nokia_tune();
        break;
    case 4:
        mario_theme();
        break;
    case 5:
        good_bad_ugly();
        break;
    default:
        break;
    }

    ESP_LOGI(TAG, "Setup task initialized");
}
