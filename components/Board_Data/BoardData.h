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


void set_valve1_state(uint8_t state);
void set_valve2_state(uint8_t state);

typedef struct {
    uint32_t commandNum;
    int32_t commandArg;
} DataFromObc;

typedef enum {
    INIT_PERIOD = 4000,
    IDLE_PERIOD = 4000,
    RECOVERY_ARM_PERIOD = 4000,
    FUELING_PERIOD = 500,
    PRESSURIZING_PERIOD = 100,
    ARMED_TO_LAUNCH_PERIOD = 1000,
    RDY_TO_LAUNCH_PERIOD = 1000,
    COUNTDOWN_PERIOD = 500,
    LIFT_OFF_PERIOD = 100,
    BURN_PERIOD = 100,
    FLIGHT_PERIOD = 100,
    FIRST_STAGE_REC_PERIOD = 250,
    SECOND_STAGE_REC_PERIOD = 500,
    ON_GROUND_PERIOD = 4000,
    HOLD_PERIOD = 1000,
    ABORT_PERIOD = 4000,
  } Periods;

  typedef enum {
    INIT = 0,
    IDLE,
    RECOVERY_ARM,
    FUELING,
    PRESSURIZING,
    ARMED_TO_LAUNCH,
    RDY_TO_LAUNCH,
    COUNTDOWN,
    LIFT_OFF,
    BURN,
    FLIGHT,
    FIRST_STAGE_RECOVERY,
    SECOND_STAGE_RECOVERY,
    ON_GROUND,
    HOLD,
    ABORT,
    NO_CHANGE = 0xff  // DO NOT USE, ONLY FOR REQUEST PURPOSE
  } States;

typedef struct DataToObc {
    bool waken_up : 1;
    uint8_t  valve1_state : 2; // 0 - closed, 1 - open
    uint8_t  valve2_state : 2; // 0 - closed, 1 - open
    int16_t temperature1;
    int16_t temperature2;
    int16_t temperature3;
    uint16_t pressure1;
    uint16_t pressure2;
    float battery_voltage;

} DataToObc;




typedef struct { 
    DataFromObc dataFromObc;
    DataToObc dataToObc;
    uint8_t obcState;
    uint16_t stateTimes[16];
} ModuleData;


esp_err_t board_data_init(void);

extern volatile ModuleData moduleData;