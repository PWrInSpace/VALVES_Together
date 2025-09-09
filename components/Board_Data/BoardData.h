#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "stdbool.h"
#include "Solenoid.h"
#include "solenoid_config.h"
#include "servo_config.h"


typedef struct {
    float temperature[2];
    uint32_t pressure[2];
    Servo_work_state_t servo_states[SERVO_COUNT];
    ValveState solenoid_states[NUM_OF_SOLENOIDS];

} BoardData_t;

extern volatile BoardData_t boardData;
extern SemaphoreHandle_t BoardDataSemaphore;


typedef struct {
    uint32_t commandNum;
    int32_t commandArg;
} DataFromObc;


typedef struct DataToObc {
    uint8_t  valve1_state : 1; // 0 - closed, 1 - open
    uint8_t  valve2_state : 1; // 0 - closed, 1 - open
    uint32_t pressure1; 
    uint32_t pressure2;

} DataToObc;

typedef struct { //TODO minimalna funckjonalnosc na razie 
    DataFromObc dataFromObc;
    DataToObc dataToObc;
    bool inServiceMode;
} ModuleData;


extern volatile ModuleData moduleData;