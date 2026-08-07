#ifndef SOLENOID_CONFIG_H
#define SOLENOID_CONFIG_H

#include "Solenoid.h"
#include "mcu_pinout.h"
#include "valve_board_config.h"

#ifdef SOL_ETH_CONFIG

typedef enum {
  ETH_FILL_SOL,
  NUM_OF_SOLENOIDS // Count of solenoids
} ValveName;

static const gpio_num_t VALVE_GPIO_PINS[NUM_OF_SOLENOIDS] = {
    [ETH_FILL_SOL] = _VALVE1_GPIO,
};

#elif defined(SOL_N2_CONFIG)

typedef enum {
  N2_FILL_SOL,
  NUM_OF_SOLENOIDS // Count of solenoids
} ValveName;

static const gpio_num_t VALVE_GPIO_PINS[NUM_OF_SOLENOIDS] = {
    [N2_FILL_SOL] = _VALVE1_GPIO,
};

#elif defined(SOL_N20_SERVO_ETH_CONFIG)

typedef enum {
  // DUMMY_SOL, // Placeholder for index 0
  N20_FILL_SOL,
  NUM_OF_SOLENOIDS
} ValveName;

static const gpio_num_t VALVE_GPIO_PINS[NUM_OF_SOLENOIDS] = {
    [N20_FILL_SOL] = _VALVE1_GPIO,

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