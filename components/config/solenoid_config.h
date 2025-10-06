#ifndef SOLENOID_CONFIG_H
#define SOLENOID_CONFIG_H

#include "valve_board_config.h"
#include "Solenoid.h"

#ifdef SOL_N2O_N2_CONFIG

typedef enum {
    N20_FILL_SOL,
    N2_FILL_SOL,
    NUM_OF_SOLENOIDS // Count of solenoids
} ValveName;

static const gpio_num_t VALVE_GPIO_PINS[NUM_OF_SOLENOIDS] = {
    [N20_FILL_SOL] = GPIO_NUM_12,
    [N2_FILL_SOL]  = GPIO_NUM_14,
};

#elif defined(SOL_ETH_CONFIG)

typedef enum {
    ETH_FILL_SOL,
    NUM_OF_SOLENOIDS // Count of solenoids
} ValveName;

static const gpio_num_t VALVE_GPIO_PINS[NUM_OF_SOLENOIDS] = {
    [ETH_FILL_SOL] = GPIO_NUM_12,
};

#elif defined(SERVO_N20_CONFIG)

typedef enum {
    NUM_OF_SOLENOIDS // No solenoids here
} ValveName;

static const gpio_num_t VALVE_GPIO_PINS[NUM_OF_SOLENOIDS] = {};

#elif defined(SERVO_ETH_N2_CONFIG)

typedef enum {
    NUM_OF_SOLENOIDS // No solenoids here
} ValveName;

static const gpio_num_t VALVE_GPIO_PINS[NUM_OF_SOLENOIDS] = {};

#else
#error "No solenoid config defined"
#endif




extern Valve valves[NUM_OF_SOLENOIDS];

esp_err_t valves_init(void);

#endif // SOLENOID_CONFIG_H