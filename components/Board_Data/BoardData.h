#ifndef BOARD_DATA_H
#define BOARD_DATA_H

#include "Solenoid.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "servo_config.h"
#include "solenoid_config.h"
#include "stdbool.h"

typedef struct {
  float vbat;
  float vin;
  float ibat;
  float iin;
  float die_temp;
  float vout;
  bool charger_status;
  bool charger_state;

} ChargerData_t;

typedef struct {
  uint64_t power_time;
  float temperature[3];
  float pressure[4];
  float termistor;
  bool dump_valve_arm;
  bool dump_valve_cont;
  bool is_charging;
  ChargerData_t chargerData;

} BoardData_t;

extern volatile uint8_t valve1_state;
extern volatile uint8_t valve2_state;
extern BoardData_t boardData;
extern SemaphoreHandle_t BoardDataSemaphore;

typedef struct {
  uint32_t commandNum;
  int32_t commandArg;
} DataFromObc;

typedef struct {
  uint32_t commandNum;
  int32_t arg1;
  int32_t arg2;
} DataFromObc2; // to test purposes

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
  NO_CHANGE = 0xff // DO NOT USE, ONLY FOR REQUEST PURPOSE
} States;

typedef struct DataToObc {

  int valve1_state;
  int valve2_state;
  int16_t temperature1;
  float pressure1;
  float pressure2;
  float battery_voltage;
  float bettery_consumption;
  float charger_temperature;
} DataToObc;

typedef struct {
  DataFromObc dataFromObc;
  DataToObc dataToObc;
  uint8_t obcState;
  uint16_t stateTimes[16];
} ModuleData;

esp_err_t board_data_init(void);
uint64_t power_time();

extern volatile ModuleData moduleData;

#endif // BOARD_DATA_H