#ifndef VALVE_BOARD_CONFIG_H
#define VALVE_BOARD_CONFIG_H

#define DUMP_VALVE_TIME_MS 1000

// SETUP
//  --------------------------------------------------------------
//                      butla N2
//  --------------------------------------------------------------
//  SCENARIO 1 - dwie płytki w górnej klatce
//
//  PCB 1                               | SOL_ETH_SERVO_N2_CONFIG
//  SOLENOID ETH                        | GPIO36    valve1_state
//  SERVO N2                            | GPIO35    valve2_state
//
//  PCB 2                               | SOL_N2_CONFIG
//  SOLENOID N2                         | GPIO36    valve1_state
//
//////////////////////////////////////////////////////////////////
//  SCENARIO 2 - jedna płytka w górnej klatce
//
//  PCB 1                               | SOL_ETH_N2_SERVO_N2_CONFIG
//  SOLENOID ETH                        | GPIO36    valve1_state
//  SERVO N2                            | GPIO35    valve2_state
//  SOLENOID N2                         | GPIO37    valve3_state
//
//  --------------------------------------------------------------
//                      butla ETH
//  --------------------------------------------------------------
//                                      | SOL_N20_SERVO_ETH_CONFIG
//  SOLENOID N2O                        | GPIO36    valve1_state
//  SERVO N20                           | GPIO35    valve2_state
//  --------------------------------------------------------------
//                      butla N20
//  --------------------------------------------------------------
//                                      | SERVO_N20_CONFIG
//  SERVO N20                           | GPIO36    valve1_state

// #define SERVO_N20_CONFIG // 1
// #define SOL_N20_SERVO_ETH_CONFIG // 2
// #define SOL_ETH_SERVO_N2_CONFIG // 3
#define SOL_N2_CONFIG // 4

#ifdef SOL_N20_SERVO_ETH_CONFIG
#define CONFIG_NAME "SOL_N20_SERVO_ETH_CONFIG"
#elif defined(SOL_ETH_SERVO_N2_CONFIG)
#define CONFIG_NAME "SOL_ETH_SERVO_N2_CONFIG"
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
