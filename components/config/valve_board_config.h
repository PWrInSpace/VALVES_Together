#ifndef VALVE_BOARD_CONFIG_H
#define VALVE_BOARD_CONFIG_H

//Choose one of the configurations below by uncommenting it

// #define SERVO_N20_CONFIG
    // #define SERVO_ETH_N2_CONFIG
// #define SOL_ETH_CONFIG
#define SOL_N2O_N2_CONFIG


// #define PRESSURE_SENSOR_SPS 100000 //when not using
#define SD_SAMPLE_RATE 100 //in Hz
#define PRESSURE_SENSOR_SPS 110
#define VOLTAGE_MEASURE_SPS 10
#define TEMPERATURE_MEASURE_SPS 10
#define TIMESTAMPS_MEASURE_SPS 100
// #define PRESSURE_CALIBRATION

#endif // VALVE_BOARD_CONFIG_H
