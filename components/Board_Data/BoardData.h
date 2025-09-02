#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "stdbool.h"
#include "Solenoid.h"
#include "solenoid_config.h"
#include "servo_config.h"


typedef struct {
    float temperature[2];
    Servo_work_state_t servo_states[SERVO_COUNT];
    ValveState solenoid_states[NUM_OF_SOLENOIDS];
} BoardData_t;

extern BoardData_t BoardData;
extern SemaphoreHandle_t BoardDataSemaphore;


typedef struct {
    uint32_t commandNum;
    int32_t commandArg;
} DataFromObc;

typedef struct { //TODO minimalna funckjonalnosc na razie 
    DataFromObc dataFromObc;
    bool inServiceMode;
} ModuleData;

extern volatile ModuleData moduleData;