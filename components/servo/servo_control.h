/**
 * @file servo_control.h
 * @author Michal Kos
 * @brief Servo control interface for multiple servos
 * @version 0.2
 * @date 2025-07-01
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef SERVO_CONTROL_HH
#define SERVO_CONTROL_HH
#include "valve_board_config.h"

/************************** SERVO SETTINGS ***********************************/

#define SERVO_MIN_PULSEWIDTH_US 500   // Minimum pulse width in microsecond
#define SERVO_MAX_PULSEWIDTH_US 2500  // Maximum pulse width in microsecond
#define SERVO_MIN_ANGLE 0             // Minimum angle
#define SERVO_MAX_ANGLE 180           // Maximum angle
#define SERVO_FREQUENCY_HZ 1000000    // Frequency of the PWM signal for the servo
#define SERVO_TIMEBASE_PERIOD 20000   // 20000 ticks, 20ms

#define DEFAULT_POS_ANGLE 90  // Angle of the default position for servos

#ifdef SERVO_N20_CONFIG
#define VALVE_CLOSE_POSITION 147U
#define VALVE_OPEN_POSITION 40U
#elif defined(SPES3D_SERVO_N20_CONFIG)
#define VALVE_CLOSE_POSITION 137U
#define VALVE_OPEN_POSITION 37U
#elif defined(SPES3D_SERVO_ETH_CONFIG)
#define VALVE_CLOSE_POSITION 170U
#define VALVE_OPEN_POSITION 50U
#elif defined(SERVO_N2_CONFIG)
#define VALVE_CLOSE_POSITION 30U
#define VALVE_OPEN_POSITION 127U
#elif defined(SOL_N20_SERVO_ETH_CONFIG)  
#define VALVE_CLOSE_POSITION 120U
#define VALVE_OPEN_POSITION 20U
#else
#define VALVE_CLOSE_POSITION 0U
#define VALVE_OPEN_POSITION 0U
#endif
#define MOVE_WITHOUT_TIMER (uint16_t)0

#include "valve_board_config.h"
#include "driver/mcpwm_prelude.h"
#include "esp_log.h"
#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>

typedef enum{
  SERVO_OPEN,
  SERVO_CLOSED
}Servo_state_t;

typedef struct{
  uint8_t angle;
  Servo_state_t state;
}Servo_work_state_t;
/************************** SERVO IDENTIFIERS ********************************/

#ifdef SERVO_N20_CONFIG
typedef enum {
    N2O_FILL_SERVO,
    SERVO_COUNT // Number of servos
} ServoId_t;
#elif defined(SPES3D_SERVO_N20_CONFIG)
typedef enum {
    N2O_FILL_SERVO,
    SERVO_COUNT // Number of servos
} ServoId_t;

#elif defined(SPES3D_SERVO_ETH_CONFIG)
typedef enum {
    ETH_FILL_SERVO,
    SERVO_COUNT // Number of servos
} ServoId_t;

#elif defined(SERVO_N2_CONFIG)
typedef enum {
    N2_FILL_SERVO,
    SERVO_COUNT // Number of servos
} ServoId_t;

#elif defined(SOL_N20_SERVO_ETH_CONFIG)
typedef enum {
    ETH_FILL_SERVO,
    SERVO_COUNT // Number of servos
} ServoId_t;

#elif defined(SOL_N2_ETH_CONFIG)
typedef enum {
    SERVO_COUNT // Number of servos
} ServoId_t;

#endif

/************************** PUBLIC VARIABLES **********************************/
typedef struct Servo {
    uint32_t pwm_pin; /**< GPIO connects to the PWM signal line */
    mcpwm_timer_handle_t timer;
    mcpwm_oper_handle_t oper;
    mcpwm_cmpr_handle_t comparator;
    mcpwm_gen_handle_t generator;
    Servo_work_state_t state;
    TimerHandle_t close_timer;
} Servo_t;

/************************** INIT MACROS **************************************/

#define SERVO_INIT(X)                                              \
  {                                                                \
    .pwm_pin = X, .timer = NULL, .oper = NULL, .comparator = NULL, \
    .generator = NULL                                              \
  }



/**
 * @brief Initialize a servo
 * @param servo_id Identifier of the servo (N20_FILL_SERVO or N2_FILL_SERVO)
 * @return uint16_t Status code (0 for success, non-zero for error)
 */
uint16_t servo_init(ServoId_t servo_id);

/**
 * @brief Move a servo to a specified angle
 * @param servo_id Identifier of the servo (N20_FILL_SERVO or N2_FILL_SERVO)
 * @param angle Target angle (between SERVO_MIN_ANGLE and SERVO_MAX_ANGLE)
 * @return uint16_t Status code (0 for success, non-zero for error)
 */
esp_err_t move_servo(ServoId_t servo_id, uint8_t angle, uint16_t open_time_ms);

/**
 * 
 */

 esp_err_t open_servo(ServoId_t servo_id, uint16_t open_time_ms);
 esp_err_t close_servo(ServoId_t servo_id);

#endif // SERVO_CONTROL_HH