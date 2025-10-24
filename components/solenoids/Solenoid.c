#include "Solenoid.h"
#include "BoardData.h"
#include <esp_err.h>
#include <driver/gpio.h>
#include "esp_timer.h"

esp_err_t valve_init(Valve *valve) {
    esp_err_t err = ESP_OK;
    valve->state = VALVE_OFF;
    valve->gpio_pin = VALVE_GPIO_PINS[valve->name];
    if (!GPIO_IS_VALID_OUTPUT_GPIO(valve->gpio_pin)) 
    {
        return ESP_ERR_INVALID_ARG;
    }

    gpio_config_t io_conf = 
    {
    .pin_bit_mask = (1ULL << valve->gpio_pin),
    .mode = GPIO_MODE_OUTPUT,
    .pull_up_en = GPIO_PULLUP_DISABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type = GPIO_INTR_DISABLE
    };

    err = gpio_config(&io_conf);
    if (err != ESP_OK) 
    {
        return err;
    }

    err = gpio_set_level(valve->gpio_pin, valve->state);
    if (err != ESP_OK) 
    {
        return err;
    }
    
    return ESP_OK;
}

esp_err_t set_valve_state(int name, ValveState state) {
    // Validate valve name
    if (name >= NUM_OF_SOLENOIDS) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err = gpio_set_level(valves[name].gpio_pin, state);
    
    return err;
}
static void timer_callback(void *arg) {
    ValveName *valve_name = (ValveName *)arg;
    if (valve_name == NULL) {
        ESP_LOGE("Solenoid", "Timer callback received NULL argument");
        return;
    }
    // Zamknij zawór po czasie
    esp_err_t err = set_valve_state(*valve_name, VALVE_OFF);
    if (err != ESP_OK) {
        ESP_LOGE("Solenoid", "Failed to close solenoid %d: %s", *valve_name, esp_err_to_name(err));
    } else {
        ESP_LOGI("Solenoid", "Solenoid %d closed after timeout", *valve_name);
    }

    if(*valve_name ==  0)
      moduleData.dataToObc.valve1_state = 0;
    else if(*valve_name == 1)
      moduleData.dataToObc.valve2_state = 0;

    free(valve_name); // Zwolnij pamięć
}

static void timer_open_callback(void *arg) {
    ValveName *valve_name = (ValveName *)arg;
    if (valve_name == NULL) {
        ESP_LOGE("Solenoid", "Timer callback received NULL argument");
        return;
    }
    // Otwórz zawór po czasie
    esp_err_t err = set_valve_state(*valve_name, VALVE_ON);
    if (err != ESP_OK) {
        ESP_LOGE("Solenoid", "Failed to open solenoid %d: %s", *valve_name, esp_err_to_name(err));
    } else {
        ESP_LOGI("Solenoid", "Solenoid %d opened after timeout", *valve_name);
    }

    if(*valve_name ==  0)
      moduleData.dataToObc.valve1_state = 1;
    else if(*valve_name == 1)
      moduleData.dataToObc.valve2_state = 1;

    free(valve_name); // Zwolnij pamięć
}

esp_err_t open_solenoid(int valve_name, uint16_t time_ms) {
    if (valve_name >= NUM_OF_SOLENOIDS) {
        ESP_LOGE("Solenoid", "Invalid valve name: %d", valve_name);
        return ESP_ERR_INVALID_ARG;
    }

    if (time_ms == 0) {
        // Otwórz bez timera
        return set_valve_state(valve_name, VALVE_ON);
    }

    // Przygotuj argument dla timera
    ValveName *timer_arg = malloc(sizeof(ValveName));
    if (!timer_arg) {
        return ESP_ERR_NO_MEM;
    }
    *timer_arg = valve_name;

    // Utwórz jednorazowy timer
    const esp_timer_create_args_t timer_args = {
        .callback = timer_callback,
        .arg = timer_arg,
        .name = "sol_timer"
    };

    esp_timer_handle_t timer_handle;
    if (esp_timer_create(&timer_args, &timer_handle) != ESP_OK) {
        free(timer_arg);
        ESP_LOGE("Solenoid", "Failed to create timer for valve %d", valve_name);
        return ESP_FAIL;
    }

    // Otwórz od razu
    esp_err_t err = set_valve_state(valve_name, VALVE_ON);
    if (err != ESP_OK) {
        esp_timer_delete(timer_handle);
        free(timer_arg);
        return err;
    }

    // Wystartuj timer (czas w mikrosekundach)
    esp_timer_start_once(timer_handle, time_ms * 1000ULL);

    return ESP_OK;
}
esp_err_t close_sol(int name) {
    if (name >= NUM_OF_SOLENOIDS) {
        return ESP_ERR_INVALID_ARG;
    }

    return set_valve_state(name, VALVE_OFF);
}



esp_err_t close_sol_time(int valve_name, uint16_t time_ms) {
    if (valve_name >= NUM_OF_SOLENOIDS) {
        ESP_LOGE("Solenoid", "Invalid valve name: %d", valve_name);
        return ESP_ERR_INVALID_ARG;
    }

    if (time_ms == 0) {
        // Zamknij bez timera
        return set_valve_state(valve_name, VALVE_OFF);
    }

    // Przygotuj argument dla timera
    ValveName *timer_arg = malloc(sizeof(ValveName));
    if (!timer_arg) {
        return ESP_ERR_NO_MEM;
    }
    *timer_arg = valve_name;

    // Utwórz jednorazowy timer
    const esp_timer_create_args_t timer_args = {
        .callback = timer_open_callback,
        .arg = timer_arg,
        .name = "sol_open_timer"
    };

    esp_timer_handle_t timer_handle;
    if (esp_timer_create(&timer_args, &timer_handle) != ESP_OK) {
        free(timer_arg);
        ESP_LOGE("Solenoid", "Failed to create timer for valve %d", valve_name);
        return ESP_FAIL;
    }

    // Zamknij od razu
    esp_err_t err = set_valve_state(valve_name, VALVE_OFF);
    if (err != ESP_OK) {
        esp_timer_delete(timer_handle);
        free(timer_arg);
        return err;
    }

    // Wystartuj timer (czas w mikrosekundach)
    esp_timer_start_once(timer_handle, time_ms * 1000ULL);

    return ESP_OK;
}


    