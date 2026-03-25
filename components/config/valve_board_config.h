#ifndef VALVE_BOARD_CONFIG_H
#define VALVE_BOARD_CONFIG_H

// SETUP NOMINALNY
//  -------------
//
//      N2
//
//
//  -------------
//
//  SERVO N2      | SERVO_N2_CONFIG              --GPIO47    valve1_state
//  SOLENOID N2         |                        --GPIO48    valve2_state
//  SOLENOID ETH        | SOL_N2_ETH_CONFIG      --GPIO47    valve1_state
//
//  -------------
//      ETH
//  -------------
//
//  SOLENOID N2O    |                            --GPIO48    valve1_state
//  SERVO ETH       | SOL_N20_SERVO_ETH_CONFIG   --GPIO47    valve2_state
//  -------------
//
//
//
//      N20
//
//
//  -------------
//
//  SERVO N20       | SERVO_N20_CONFIG           --GPIO47    valve1_state
//

// #define SERVO_N20_CONFIG
// #define SERVO_N2_CONFIG
#define SOL_N20_SERVO_ETH_CONFIG
// #define SOL_N2_ETH_CONFIG

#ifdef SOL_N20_SERVO_ETH_CONFIG
#define CONFIG_NAME "SOL_N20_SERVO_ETH_CONFIG"
#elif defined(SOL_N2_ETH_CONFIG)
#define CONFIG_NAME "SOL_N2_ETH_CONFIG"
#elif defined(SERVO_N20_CONFIG)
#define CONFIG_NAME "SERVO_N20_CONFIG"
#elif defined(SERVO_N2_CONFIG)
#define CONFIG_NAME "SERVO_N2_CONFIG"
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
Charging data DONE
Dump Valve DONE
SD DONE

Buzzer napiany trzeba dodac nutki
Press
Thermocouple
RGB
*/