#ifndef SOLENOID_H
#define SOLENOID_H

#include <driver/gpio.h>

typedef enum {
    VALVE_OFF = 0,
    VALVE_ON = 1
} ValveState;

typedef struct {
    int name; //enum
    ValveState state;
    gpio_num_t gpio_pin;
} Valve;

esp_err_t valve_init(Valve *valve);
esp_err_t set_valve_state(int name, ValveState state);
esp_err_t open_solenoid(int name, uint16_t time_ms);
esp_err_t close_sol(int name);

#endif // SOLENOID_H