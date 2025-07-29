#include "timers_config.h"
#include "esp_log.h"

#define TAG "TIMERS"

void test_timer_callback(void* arg) {
    ESP_LOGI(TAG, "Test timer callback executed");
}

bool timers_init(void) {
    sys_timer_t timers[] = {
        {
            .timer_id = 0,
            .timer_callback_fnc = test_timer_callback,
            .timer_arg = NULL
        }
    };

    return sys_timer_init(timers, sizeof(timers) / sizeof(timers[0]));
}

bool start_test_timer(void) {
    return sys_timer_start(0, 5, TIMER_TYPE_PERIODIC);
}