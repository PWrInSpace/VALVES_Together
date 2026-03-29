#ifndef VALVE_BOARD_CONFIG_H
#define VALVE_BOARD_CONFIG_H

#define DUMP_VALVE_TIME_MS 1000

// SETUP NOMINALNY
//  -------------
//      N2
//  -------------
//
//  SOLENOID N2         | SOL N2_CONFIG       --GPIO47    valve1_state - brak pressure
//
//  SOLENOID ETH        | SOL_ETH_CONFIG      --GPIO47    valve1_state - press1 to press N2 
//
//  -------------
//      ETH
//  -------------
//
//  SOLENOID N2O    |                            --GPIO48    valve1_state   -press1 to press N2O
//  SERVO ETH       | SOL_N20_SERVO_ETH_CONFIG   --GPIO47    valve2_state   -press2 to press ETH
//  -------------
//
//
//      N20
//
//  -------------
//
//  SERVO N20       | SERVO_N20_CONFIG           --GPIO47    valve1_state
//

#define SERVO_N20_CONFIG
// #define SOL_N20_SERVO_ETH_CONFIG
// #define SOL_ETH_CONFIG
// #define SOL_N2_CONFIG

#ifdef SOL_N20_SERVO_ETH_CONFIG
#define CONFIG_NAME "SOL_N20_SERVO_ETH_CONFIG"
#elif defined(SOL_ETH_CONFIG)
#define CONFIG_NAME "SOL_ETH_CONFIG"
#elif defined(SERVO_N20_CONFIG)
#define CONFIG_NAME "SERVO_N20_CONFIG"
#elif defined(SOL_N2_CONFIG)
#define CONFIG_NAME "SOL_N2_CONFIG"
#else
#error "No valve configuration defined! Please define one of: SERVO_N20_CONFIG"
#endif

// #define PRESSURE_CALIBRATION
// #define THERMISTOR_CALIBRATION

#endif // VALVE_BOARD_CONFIG_H

/*TODO
Cli Napisane ale trzeba uzupelniac na bierzaco
I2C scanner DONE
Servo DONE
Solenoid DONE
Charging data dodac taska
Dump Valve DONE
SD DONE

Buzzer
Press
Thermocouple
RGB
*/