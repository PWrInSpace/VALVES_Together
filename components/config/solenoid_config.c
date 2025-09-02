#include "solenoid_config.h"
#include "Solenoid.h"
#include "esp_log.h"

#ifdef SOL_N2O_N2_CONFIG


Valve valves[NUM_OF_SOLENOIDS] = {
    [N20_FILL_SOL] = {.name = N20_FILL_SOL},
    [N2_FILL_SOL] = {.name = N2_FILL_SOL},
};

#elif defined(SOL_ETH_CONFIG)

Valve valves[NUM_OF_SOLENOIDS] = {
    [ETH_FILL_SOL] = {.name = ETH_FILL_SOL},
};

#elif defined(SERVO_N20_CONFIG)

Valve valves[NUM_OF_SOLENOIDS] = {};
#elif defined(SERVO_ETH_N2_CONFIG)

Valve valves[NUM_OF_SOLENOIDS] = {};
#else
#error "No solenoid config defined"
#endif


esp_err_t valves_init()
{
    esp_err_t ret = 0;
    for(int i=0; i<NUM_OF_SOLENOIDS; i++)
    {
        ret |= valve_init(&valves[i]);
    }
    return ret;
}

