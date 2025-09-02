#include "Solenoid.h"
#include "BoardData.h"
#include <esp_err.h>
#include <driver/gpio.h>

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