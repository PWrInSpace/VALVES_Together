/**
 * @file servo_control.c
 * @author Michal Kos
 * @brief Servo control interface for multiple servos
 * @version 0.2
 * @date 2025-07-01
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "servo_control.h"
#include "servo_config.h"
#include "freertos/FreeRTOS.h"

/************************** PRIVATE INCLUDES ********************************/

#include "esp_log.h"
#include "BoardData.h"

/************************** PRIVATE VARIABLES *******************************/

static const char *TAG = "SERVO CONTROL";

// Servo configurations

/************************** PRIVATE FUNCTIONS *******************************/

static inline uint32_t angle_to_duty_us(uint8_t angle) {
  return (angle - SERVO_MIN_ANGLE) *
             (SERVO_MAX_PULSEWIDTH_US - SERVO_MIN_PULSEWIDTH_US) /
             (SERVO_MAX_ANGLE - SERVO_MIN_ANGLE) +
         SERVO_MIN_PULSEWIDTH_US;
}


static void close_servo_callback(TimerHandle_t xTimer) {
  ServoId_t servo_id = (ServoId_t)pvTimerGetTimerID(xTimer);
  Servo_t *servo_ptr = &servos[servo_id];

  if (mcpwm_comparator_set_compare_value(servo_ptr->comparator, angle_to_duty_us(VALVE_CLOSE_POSITION)) != ESP_OK) {
    // ESP_LOGE(TAG, "Failed to close servo %d", servo_id);
  } else {
    vTaskDelay(pdMS_TO_TICKS(300)); // Short delay to ensure the servo has time to move
    if(servo_ptr->state.angle > VALVE_CLOSE_POSITION){
      mcpwm_comparator_set_compare_value(servo_ptr->comparator, angle_to_duty_us(VALVE_CLOSE_POSITION + 5));
    }
    else if(servo_ptr->state.angle <= VALVE_CLOSE_POSITION){
      mcpwm_comparator_set_compare_value(servo_ptr->comparator, angle_to_duty_us(VALVE_CLOSE_POSITION - 5));
    }
    servo_ptr->state.state = SERVO_CLOSED;
    servo_ptr->state.angle = VALVE_CLOSE_POSITION;
    if(servo_id == 0)
      valve1_state = 0;
    else if(servo_id == 1)
      valve2_state = 0;
   ESP_LOGI(TAG, "Servo %d closed after timeout", servo_id);
  }
}
/************************** CODE *********************************************/

uint16_t servo_init(ServoId_t servo_id) {
  if (servo_id >= SERVO_COUNT) {
    ESP_LOGE(TAG, "Invalid servo ID: %d", servo_id);
    return EXIT_FAILURE;
  }

  Servo_t *servo_ptr = &servos[servo_id];

  // Create timer
  mcpwm_timer_config_t timer_config = {
      .group_id = servo_id, // Use unique group ID for each servo
      .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
      .resolution_hz = SERVO_FREQUENCY_HZ,
      .period_ticks = SERVO_TIMEBASE_PERIOD,
      .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
  };
  ESP_ERROR_CHECK(mcpwm_new_timer(&timer_config, &servo_ptr->timer));

  // Create operator
  mcpwm_operator_config_t operator_config = {
      .group_id = servo_id, // Operator in the same group as the timer
  };
  ESP_ERROR_CHECK(mcpwm_new_operator(&operator_config, &servo_ptr->oper));

  // Connect operator to timer
  ESP_ERROR_CHECK(
      mcpwm_operator_connect_timer(servo_ptr->oper, servo_ptr->timer));

  // Create comparator
  mcpwm_comparator_config_t comparator_config = {
      .flags.update_cmp_on_tez = true,
  };
  ESP_ERROR_CHECK(mcpwm_new_comparator(servo_ptr->oper, &comparator_config,
                                       &servo_ptr->comparator));

  // Create generator
  mcpwm_generator_config_t generator_config = {
      .gen_gpio_num = servo_ptr->pwm_pin,
  };
  ESP_ERROR_CHECK(mcpwm_new_generator(servo_ptr->oper, &generator_config,
                                      &servo_ptr->generator));

  // Set the initial compare value, so that the servo will spin to the center
  ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(
      servo_ptr->comparator, angle_to_duty_us(VALVE_CLOSE_POSITION)));

  // Set generator action on timer and compare event
  // Go high on counter empty
  ESP_ERROR_CHECK(mcpwm_generator_set_actions_on_timer_event(
      servo_ptr->generator,
      MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP,
                                   MCPWM_TIMER_EVENT_EMPTY,
                                   MCPWM_GEN_ACTION_HIGH),
      MCPWM_GEN_TIMER_EVENT_ACTION_END()));

  // Go low on compare threshold
  ESP_ERROR_CHECK(mcpwm_generator_set_actions_on_compare_event(
      servo_ptr->generator,
      MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP,
                                     servo_ptr->comparator,
                                     MCPWM_GEN_ACTION_LOW),
      MCPWM_GEN_COMPARE_EVENT_ACTION_END()));


  servo_ptr->close_timer = xTimerCreate(
      "CLOSE_SERVO_TIMER",              // Timer name
      pdMS_TO_TICKS(1000),    // Default period (will be updated in move_servo)
      pdFALSE,                // One-shot timer (not auto-reload)
      (void *)(intptr_t)servo_id, // Timer ID (servo_id)
      close_servo_callback     // Callback function
  );
  if (servo_ptr->close_timer == NULL) {
    ESP_LOGE(TAG, "Failed to create timer for servo %d", servo_id);
    return EXIT_FAILURE;
  }

  mcpwm_timer_enable(servo_ptr->timer);
  mcpwm_timer_start_stop(servo_ptr->timer, MCPWM_TIMER_START_NO_STOP);

  //mcpwm_generator_set_force_level(servo_ptr->generator, 1 , false);

  ESP_LOGI(TAG, "Servo %d initialized on GPIO %d", servo_id, servo_ptr->pwm_pin);
  return EXIT_SUCCESS;
}


esp_err_t move_servo(ServoId_t servo_id, uint8_t angle, uint16_t open_time_ms) {
  if (servo_id >= SERVO_COUNT) {
    ESP_LOGE(TAG, "Invalid servo ID: %d", servo_id);
    return ESP_LOG_ERROR;
  }

  Servo_t *servo_ptr = &servos[servo_id];
  ESP_LOGI(TAG, "Moving servo[%d] to angle: %d", servo_id, angle);

  // mcpwm_timer_enable(servo_ptr->timer);
  // mcpwm_timer_start_stop(servo_ptr->timer, MCPWM_TIMER_START_NO_STOP);

  if (mcpwm_comparator_set_compare_value(servo_ptr->comparator, angle_to_duty_us(angle)) != ESP_OK) {
    ESP_LOGE(TAG, "Moving servo %d FAILURE", servo_id);
    return ESP_LOG_ERROR;
  }

  vTaskDelay(pdMS_TO_TICKS(300)); // Short delay to ensure the servo has time to move
  if(servo_ptr->state.angle > angle){
    mcpwm_comparator_set_compare_value(servo_ptr->comparator, angle_to_duty_us(angle + 5));
  }
  else if(servo_ptr->state.angle <= angle){
    mcpwm_comparator_set_compare_value(servo_ptr->comparator, angle_to_duty_us(angle - 5));
  }
  servo_ptr->state.angle = angle;
  servo_ptr->state.state = SERVO_OPEN;
  if (xTimerIsTimerActive(servo_ptr->close_timer) != pdFALSE) {
    xTimerStop(servo_ptr->close_timer, 0);
  }
  if (open_time_ms > 0) {
    xTimerChangePeriod(servo_ptr->close_timer, pdMS_TO_TICKS(open_time_ms), 0);
    xTimerStart(servo_ptr->close_timer, 0);
  }

  //mcpwm_generator_set_force_level(servo_ptr->generator, 0 , false);

  return ESP_OK;
}

esp_err_t open_servo(ServoId_t servo_id, uint16_t open_time)
{
  if (servo_id >= SERVO_COUNT) {
    ESP_LOGE(TAG, "Invalid servo ID: %d", servo_id);
    return ESP_LOG_ERROR;
  }

  ESP_LOGI(TAG, "OPENING servo[%d] to angle: %d", servo_id, VALVE_OPEN_POSITION);

  if(move_servo(servo_id, VALVE_OPEN_POSITION, open_time)!=ESP_OK)
  {
    return ESP_LOG_ERROR;
  }
  
  ESP_LOGI(TAG, "OPENED servo[%d] to angle: %d for time ms: %d", servo_id, VALVE_OPEN_POSITION, open_time);


  return ESP_OK;
}

esp_err_t close_servo(ServoId_t servo_id)
{
  if (servo_id >= SERVO_COUNT) {
    ESP_LOGE(TAG, "Invalid servo ID: %d", servo_id);
    return ESP_LOG_ERROR;
  }

  ESP_LOGI(TAG, "CLOSING servo[%d] to angle: %d", servo_id, VALVE_CLOSE_POSITION);
  if(move_servo(servo_id, VALVE_CLOSE_POSITION, MOVE_WITHOUT_TIMER)!=ESP_OK)
  {
    return ESP_LOG_ERROR;
  }
  ESP_LOGI(TAG, "CLOSED servo[%d] to angle: %d", servo_id, VALVE_CLOSE_POSITION);

  return ESP_OK;
}