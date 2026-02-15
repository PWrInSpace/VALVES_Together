#ifndef SOLENOID_CONFIG_H
#define SOLENOID_CONFIG_H

#include "valve_board_config.h"
#include "Solenoid.h"

#ifdef SOL_N2_ETH_CONFIG

typedef enum {
    N2_FILL_SOL,
    ETH_FILL_SOL,
    NUM_OF_SOLENOIDS // Count of solenoids
} ValveName;

static const gpio_num_t VALVE_GPIO_PINS[NUM_OF_SOLENOIDS] = {
    [ETH_FILL_SOL] = GPIO_NUM_15,
    [N2_FILL_SOL]  = GPIO_NUM_16,
};

#elif defined(SOL_N20_SERVO_ETH_CONFIG)

typedef enum {
    N20_FILL_SOL,
    NUM_OF_SOLENOIDS
} ValveName;

static const gpio_num_t VALVE_GPIO_PINS[NUM_OF_SOLENOIDS] = {
    [N20_FILL_SOL] = GPIO_NUM_15,

};

#else

typedef enum {
    NUM_OF_SOLENOIDS // No solenoids here
} ValveName;

static const gpio_num_t VALVE_GPIO_PINS[NUM_OF_SOLENOIDS] = {};

#endif




extern Valve valves[NUM_OF_SOLENOIDS];

esp_err_t valves_init(void);

#endif // SOLENOID_CONFIG_H