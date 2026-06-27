#pragma once
#include "Solenoid.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "servo_config.h"
#include "solenoid_config.h"
#include "ltc4162.h"
#include "stdbool.h"
#include <inttypes.h>
#include <stdint.h>

#ifdef SOL_N20_SERVO_ETH_CONFIG
typedef struct {
  uint64_t power_time;
  float temperature[3];
  float pressure[4];
  float termistor;
  bool dump_valve_arm;
  bool dump_valve_cont;
  bool is_charging;
  bool auto_vent_activated;
  bool auto_vent_triggered;
  ltc4162_charger_data_t chargerData;
} BoardData_t;
#else

typedef struct {
  uint64_t power_time;
  float temperature[3];
  float pressure[4];
  float termistor;
  bool dump_valve_arm;
  bool dump_valve_cont;
  bool is_charging;
  ltc4162_charger_data_t chargerData;

} BoardData_t;

#endif

extern volatile uint8_t valve1_state;
extern volatile uint8_t valve2_state;

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
  FUELING_PERIOD = 1000,
  PRESSURIZING_PERIOD = 1000,
  ARMED_TO_LAUNCH_PERIOD = 1000,
  RDY_TO_LAUNCH_PERIOD = 1000,
  COUNTDOWN_PERIOD = 500,
  LIFT_OFF_PERIOD = 500,
  BURN_PERIOD = 500,
  FLIGHT_PERIOD = 500,
  FIRST_STAGE_REC_PERIOD = 500,
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

#if defined(SOL_N20_SERVO_ETH_CONFIG)
typedef struct DataToObc {
  bool waken_up : 1;
  bool dump_valve_arm : 1;  // 0 not armed, 1 armed
  bool dump_valve_cont : 1; // 0 no cont, 1 cont
  bool is_charging : 1;     // 0 not charging, 1 charging
  bool auto_vent_activated : 1;
  bool auto_vent_triggered : 1;
  int32_t auto_vent_pressure;
  int16_t ox_temperature;
  uint8_t valve1_state : 2; // 0 - closed, 1 - open
  uint8_t valve2_state : 2; // 0 - closed, 1 - open
  int16_t temperature1;
  float pressure1;
  float pressure2;
  float battery_voltage;
  float battery_consumption;
  float charger_temperature;
} DataToObc;

#else
typedef struct DataToObc {
  bool waken_up : 1;
  bool dump_valve_arm : 1;  // 0 not armed, 1 armed
  bool dump_valve_cont : 1; // 0 no cont, 1 cont
  bool is_charging : 1;     // 0 not charging, 1 charging
  uint8_t valve1_state : 2; // 0 - closed, 1 - open
  uint8_t valve2_state : 2; // 0 - closed, 1 - open
  int16_t temperature1;
  float pressure1;
  float pressure2;
  float battery_voltage;
  float battery_consumption;
  float charger_temperature;
} DataToObc;
#endif

typedef struct {
  DataFromObc dataFromObc;
  DataToObc dataToObc;
  uint8_t obcState;
  uint16_t stateTimes[16];
} ModuleData;

esp_err_t board_data_init(void);
uint64_t power_time();

esp_err_t get_board_data(BoardData_t* data, uint32_t mutexTimeout);
esp_err_t set_board_data(BoardData_t data, uint32_t mutexTimeout);

esp_err_t get_boardData_charger_data(ChargerData_t *data, uint32_t mutexTimeout);
esp_err_t set_boardData_charger_data(ChargerData_t data, uint32_t mutexTimeout);

esp_err_t get_boardData_pressures(float pressures[4], uint32_t mutexTimeout);
esp_err_t set_boardData_pressures(float pressures[4], uint32_t mutexTimeout);

esp_err_t get_boardData_temperatures(float temperatures[3], uint32_t mutexTimeout);
esp_err_t set_boardData_temperatures(float temperatures[3], uint32_t mutexTimeout);

esp_err_t get_boardData_power_time(uint64_t *power_time, uint32_t mutexTimeout);
esp_err_t set_boardData_power_time(uint64_t power_time, uint32_t mutexTimeout);

extern volatile ModuleData moduleData;