#include <string.h>

#include "esp_log.h"
#include "esp_system.h"

#include "BoardData.h"
#include "auto_vent_task.h"
#include "board_config.h"
#include "buzzer.h"
#include "commands.h"
#include "console.h"
#include "console_config.h"
#include "i2c_scan.h"
#include "igniter_driver.h"
#include "ltc4162.h"
#include "now.h"
#include "pressure_driver.h"
#include "valve_board_config.h"
#include "valves_control.h"

#define TAG "CONSOLE_CONFIG"

int reset_device(int argc, char **argv) {
  ESP_LOGI(TAG, "Resetting device...");
  esp_restart();
  return 0;
}

int run_i2c_scan(int argc, char **argv) {
  i2c_scan();
  return 0;
}

int run_ltc4162_monitor(int argc, char **argv) {
  ltc4162_debug_monitor();
  return 0;
}

int run_igniter_continuity_check(int argc, char **argv) {
  igniter_continuity_t continuity;
  ESP_LOGI(TAG, "Checking igniter continuity...");
  igniter_status_t status = igniter_check_continuity(igniter_cfg, &continuity);
  ESP_LOGI(TAG, "Continuity check status: %d", status);
  if (status != IGNITER_OK) {
    ESP_LOGE(TAG, "Igniter continuity check failed with status %d", status);
    return -1;
  }
  if (continuity == IGNITER_CONTINUITY_OK) {
    ESP_LOGI(TAG, "Igniter continuity check passed");
  } else {
    ESP_LOGW(TAG, "Igniter continuity check failed");
  }
  return 0;
}

int run_igniter_arm(int argc, char **argv) {
  igniter_status_t status = igniter_arm(igniter_cfg);
  if (status != IGNITER_OK) {
    ESP_LOGE(TAG, "Igniter arming failed with status %d", status);
    return -1;
  }
  ESP_LOGI(TAG, "Igniter armed successfully");
  return 0;
}

int run_igniter_disarm(int argc, char **argv) {
  igniter_status_t status = igniter_disarm(igniter_cfg);
  if (status != IGNITER_OK) {
    ESP_LOGE(TAG, "Igniter disarming failed with status %d", status);
    return -1;
  }
  ESP_LOGI(TAG, "Igniter disarmed successfully");
  return 0;
}

int run_igniter_fire(int argc, char **argv) {
  uint64_t fire_time = DUMP_VALVE_TIME_MS;
  if (argc == 2) {
    fire_time = atoi(argv[1]);
  }
  igniter_status_t status = igniter_fire_time(igniter_cfg, fire_time);

  if (status != IGNITER_OK) {
    ESP_LOGE(TAG, "Igniter firing failed with status %d", status);
    return -1;
  }
  ESP_LOGI(TAG, "Igniter fired successfully");

  return 0;
}

int play_imperial_march(int argc, char **argv) {
  imperial_march();
  return 0;
}

int play_ode_to_joy(int argc, char **argv) {
  ode_to_joy();
  return 0;
}

int play_single_beep(int argc, char **argv) {
  beep_single();
  return 0;
}

int play_double_beep(int argc, char **argv) {
  beep_double();
  return 0;
}

int play_triple_beep(int argc, char **argv) {
  beep_triple();
  return 0;
}

int play_quatro_beep(int argc, char **argv) {
  beep_quatro();
  return 0;
}

int open_valve1(int argc, char **argv) {
  if (argc < 2) {
    ESP_LOGE(TAG, "Usage: open_valve_name <duration_ms>");
    return -1;
  }

  int duration_ms = atoi(argv[1]);

#ifdef SOL_N20_SERVO_ETH_CONFIG
  chandle_valve_cmd(N20_SOL_OPEN, duration_ms);
#elif defined(SOL_N2_CONFIG)
  chandle_valve_cmd(N2_SOL_OPEN, duration_ms);
#elif defined(SERVO_N20_CONFIG)
  chandle_valve_cmd(N20_VALVE_OPEN, duration_ms);
#elif defined(SOL_ETH_CONFIG)
  chandle_valve_cmd(ETH_SOL_OPEN, duration_ms);
#else
  ESP_LOGE(TAG, "No valve configuration defined!");
#endif

  return 0;
}

int close_valve1(int argc, char **argv) {

#ifdef SOL_N20_SERVO_ETH_CONFIG
  chandle_valve_cmd(N20_SOL_CLOSE, 0);
#elif defined(SOL_N2_CONFIG)
  chandle_valve_cmd(N2_SOL_CLOSE, 0);
#elif defined(SERVO_N20_CONFIG)
  chandle_valve_cmd(N20_VALVE_CLOSE, 0);
#elif defined(SOL_ETH_CONFIG)
  chandle_valve_cmd(ETH_SOL_CLOSE, 0);
#else
  ESP_LOGE(TAG, "No valve configuration defined!");
#endif
  return 0;
}

int open_valve2(int argc, char **argv) {
  if (argc < 2) {
    ESP_LOGE(TAG, "Usage: open_valve_name <duration_ms>");
    return -1;
  }

  int duration_ms = atoi(argv[1]);

#ifdef SOL_N20_SERVO_ETH_CONFIG
  chandle_valve_cmd(ETH_VALVE_OPEN, duration_ms);
#elif defined(SOL_ETH_CONFIG)
  ESP_LOGI(TAG, "SOL_ETH_CONFIG does not have valve 2!");
#elif defined(SERVO_N20_CONFIG)
  ESP_LOGI(TAG, "SERVO_N20_CONFIG does not have valve 2!");
#elif defined(SOL_N2_CONFIG)
  ESP_LOGI(TAG, "SOL_N2_CONFIG does not have valve 2!");
#else
  ESP_LOGE(TAG, "No valve configuration defined!");
#endif

  return 0;
}

int close_valve2(int argc, char **argv) {

#ifdef SOL_N20_SERVO_ETH_CONFIG
  chandle_valve_cmd(ETH_VALVE_CLOSE, 0);
#elif defined(SOL_ETH_CONFIG)
  ESP_LOGI(TAG, "SOL_ETH_CONFIG does not have valve 2!");
#elif defined(SERVO_N20_CONFIG)
  ESP_LOGI(TAG, "SERVO_N20_CONFIG does not have valve 2!");
#elif defined(SOL_N2_CONFIG)
  ESP_LOGI(TAG, "SOL_N2_CONFIG does not have valve 2!");
#else
  ESP_LOGE(TAG, "No valve configuration defined!");
#endif
  return 0;
}

int play_harry_potter_theme(int argc, char **argv) {
  harry_potter_theme();
  return 0;
}

int get_board_data(int argc, char **argv) {
  int n = 1;
  if (argc == 2) {
    n = atoi(argv[1]);
  }
  for (int i = 0; i < n; i++) {

    if (xSemaphoreTake(BoardDataSemaphore, portMAX_DELAY) == pdTRUE) {
      ESP_LOGI(TAG, "-----------------------------------");
      ESP_LOGI(TAG, "Current board data:");
      ESP_LOGI(TAG, "Power time: %llu", boardData.power_time);
      ESP_LOGI(TAG, "valve1 state: %d", valve1_state);
      ESP_LOGI(TAG, "valve2 state: %d", valve2_state);
      ESP_LOGI(TAG, "Temperature: %f, %f, %f", boardData.temperature[0],
               boardData.temperature[1], boardData.temperature[2]);
      ESP_LOGI(TAG, "Pressure left channel: %f", boardData.pressure[2]);
      ESP_LOGI(TAG, "Pressure middle channel: %f", boardData.pressure[3]);
      ESP_LOGI(TAG, "Dump valve arm: %d", boardData.dump_valve_arm);
      ESP_LOGI(TAG, "Dump valve continuity: %d", boardData.dump_valve_cont);
      ESP_LOGI(TAG, "Is charging: %d", boardData.is_charging);
      ESP_LOGI(TAG, "------------------------------------\n\n");
      xSemaphoreGive(BoardDataSemaphore);
    } else {
      ESP_LOGE(TAG, "Failed to take BoardDataSemaphore");
      return -1;
    }

    vTaskDelay(pdMS_TO_TICKS(1000));
  }

  return 0;
}

int set_calibration_mode(int argc, char **argv) {
  calibration_mode = !calibration_mode;
  ESP_LOGI(TAG, "Calibration mode: %s", calibration_mode ? "ON" : "OFF");
  return 0;
}

int set_now_send_log(int argc, char **argv) {
  if (argc >= 2) {
    if (strcmp(argv[1], "on") == 0 || strcmp(argv[1], "1") == 0) {
      now_send_data_log_enabled = true;
    } else if (strcmp(argv[1], "off") == 0 || strcmp(argv[1], "0") == 0) {
      now_send_data_log_enabled = false;
    } else {
      ESP_LOGE(TAG, "Usage: now_send_log [on|off]");
      return -1;
    }
  } else {
    now_send_data_log_enabled = !now_send_data_log_enabled;
  }
  ESP_LOGI(TAG, "NOW send data log: %s",
           now_send_data_log_enabled ? "ON" : "OFF");
  return 0;
}

int set_obc_test_data(int argc, char **argv) {
  if (argc >= 2) {
    if (strcmp(argv[1], "on") == 0 || strcmp(argv[1], "1") == 0) {
      obc_test_data_enabled = true;
    } else if (strcmp(argv[1], "off") == 0 || strcmp(argv[1], "0") == 0) {
      obc_test_data_enabled = false;
    } else {
      ESP_LOGE(TAG, "Usage: obc_test_data [on|off]");
      return -1;
    }
  } else {
    obc_test_data_enabled = !obc_test_data_enabled;
  }
  ESP_LOGI(TAG, "OBC test data: %s", obc_test_data_enabled ? "ON" : "OFF");
  return 0;
}

int deinit_i2c(int argc, char **argv) {
  esp_err_t ret = mcu_i2c_deinit();
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "I2C deinitialization failed");
    return -1;
  }
  ESP_LOGI(TAG, "I2C deinitialized successfully");
  return 0;
}

int init_i2c(int argc, char **argv) {
  esp_err_t ret = mcu_i2c_init();
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "I2C initialization failed");
    return -1;
  }
  ESP_LOGI(TAG, "I2C initialized successfully");
  return 0;
}

int init_i2c_with_pins(int argc, char **argv) {
  esp_err_t ret = mcu_i2c_init_with_pins(SDA_GPIO_ALT, SCL_GPIO_ALT);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "I2C initialization with pins failed");
    return -1;
  }
  ESP_LOGI(TAG, "I2C initialized with pins successfully");
  return 0;
}

int open_angle(int argc, char **argv) {
  if (argc < 3) {
    ESP_LOGE(TAG, "Usage: open_angle <valve_id> <angle>");
    return -1;
  }
  uint8_t valve_id = atoi(argv[1]);
  int angle = atoi(argv[2]);
  chandle_valve_cmd_angle(valve_id, 0, angle);
  return 0;
}
#ifdef SOL_N20_SERVO_ETH_CONFIG
int get_auto_vent_data(int argc, char **argv) {
  float auto_vent_pressure = 0.0f;
  get_auto_vent_pressure(&auto_vent_pressure);
  ESP_LOGI(TAG, "Auto vent pressure: %f bar", auto_vent_pressure);
  ESP_LOGI(TAG, "Auto vent pressure: %d mbar",
           (int)(auto_vent_pressure * 1000));
  ESP_LOGI(TAG, "Auto vent activated: %d", is_auto_vent_active);
  ESP_LOGI(TAG, "Auto vent triggered: %d", is_triggered);
  return 0;
}
#endif

int print_board_data(int argc, char **argv) {
  BoardData_t board_data;
  if (xSemaphoreTake(BoardDataSemaphore, portMAX_DELAY) == pdTRUE) {
    memcpy(&board_data, &boardData, sizeof(BoardData_t));
    xSemaphoreGive(BoardDataSemaphore);
  }
  ESP_LOGI(TAG, "-----------------------------------");
  ESP_LOGI(TAG, "Board data:");
  ESP_LOGI(TAG, "Power time: %llu", board_data.power_time);
  ESP_LOGI(TAG, "Temperature: %f, %f, %f", board_data.temperature[0],
           board_data.temperature[1], board_data.temperature[2]);
  ESP_LOGI(TAG, "Pressure: %f, %f, %f", board_data.pressure[0],
           board_data.pressure[1], board_data.pressure[2]);
  ESP_LOGI(TAG, "Dump valve arm: %d", board_data.dump_valve_arm);
  ESP_LOGI(TAG, "Dump valve continuity: %d", board_data.dump_valve_cont);
  ESP_LOGI(TAG, "Is charging: %d", board_data.is_charging);
  ESP_LOGI(TAG, "------------------------------------\n\n");
  return 0;
}

int play_crab_rave_melody(int argc, char **argv) {
  crab_rave_melody();
  return 0;
}

int play_crab_rave_harmony(int argc, char **argv) {
  crab_rave_harmony();
  return 0;
}

int play_crab_rave_bass(int argc, char **argv) {
  crab_rave_bass();
  return 0;
}

#ifdef SOL_N20_SERVO_ETH_CONFIG
int auto_vent_on(int argc, char **argv) {
  if (argc < 2) {
    ESP_LOGE(TAG, "Usage: auto_vent_on <pressure>");
    return -1;
  }
  float pressure = atof(argv[1]);
  set_auto_vent_on(pressure);
  return 0;
}

int auto_vent_off(int argc, char **argv) {
  set_auto_vent_off();
  return 0;
}
#endif
// Place for the console configuration

// clang-format off
static esp_console_cmd_t cmd[] = {
    // example command:
    // cmd     help description   hint  function      args  func_context context
    {"reset", "Reset the device", NULL, reset_device, NULL, NULL, NULL},
    {"i2c_scan", "Scan the I2C bus for devices", NULL, run_i2c_scan, NULL, NULL, NULL},
    {"ltc_monitor", "Run LTC4162 debug monitor", NULL, run_ltc4162_monitor, NULL, NULL, NULL},
    {"igniter_continuity", "Check igniter continuity", NULL, run_igniter_continuity_check, NULL, NULL, NULL},
    {"igniter_arm", "Arm the igniter", NULL, run_igniter_arm, NULL, NULL, NULL},
    {"igniter_disarm", "Disarm the igniter", NULL, run_igniter_disarm, NULL, NULL, NULL},
    {"igniter_fire", "Fire the igniter", NULL, run_igniter_fire, NULL, NULL, NULL},
    {"play_imperial_march", "Play the Imperial March on the buzzer", NULL, play_imperial_march, NULL, NULL, NULL},
    {"play_ode_to_joy", "Play Ode to Joy on the buzzer", NULL, play_ode_to_joy, NULL, NULL, NULL},
    {"play_harry_potter_theme", "Play Harry Potter theme on the buzzer", NULL, play_harry_potter_theme, NULL, NULL, NULL},
    {"play_single_beep", "Play a single beep on the buzzer", NULL, play_single_beep, NULL, NULL, NULL},
    {"play_double_beep", "Play a double beep on the buzzer", NULL, play_double_beep, NULL, NULL, NULL},
    {"play_triple_beep", "Play a triple beep on the buzzer", NULL, play_triple_beep, NULL, NULL, NULL},
    {"play_quatro_beep", "Play a quatro beep on the buzzer", NULL, play_quatro_beep, NULL, NULL, NULL},
    {"get_board_data", "Print current board data to console", NULL, get_board_data, NULL, NULL, NULL},
    {"set_calibration_mode", "Toggle calibration mode for pressure sensors", NULL, set_calibration_mode, NULL, NULL, NULL},
    {"now_send_log", "Enable/disable ESP-NOW data-to-OBC debug log (on|off or toggle)", NULL, set_now_send_log, NULL, NULL, NULL},
    {"obc_test_data", "Enable/disable test data in ESP-NOW packets to OBC (on|off or toggle)", NULL, set_obc_test_data, NULL, NULL, NULL},
    {"deinit_i2c", "Deinitialize the I2C bus", NULL, deinit_i2c, NULL, NULL, NULL},
    {"init_i2c", "Initialize the I2C bus", NULL, init_i2c, NULL, NULL, NULL},
    {"init_i2c_with_pins", "Initialize the I2C bus with custom SDA/SCL pins", NULL, init_i2c_with_pins, NULL, NULL, NULL},
    {"open_angle", "Open a valve to a specified angle", NULL, open_angle, NULL, NULL, NULL},
    {"print_board_data", "Print current board data to console", NULL, print_board_data, NULL, NULL, NULL},
    {"play_crab_rave_melody", "Play Crab Rave Melody on the buzzer", NULL, play_crab_rave_melody, NULL, NULL, NULL},
    {"play_crab_rave_harmony", "Play Crab Rave Harmony on the buzzer", NULL, play_crab_rave_harmony, NULL, NULL, NULL},
    {"play_crab_rave_bass", "Play Crab Rave Bass on the buzzer", NULL, play_crab_rave_bass, NULL, NULL, NULL},
    

    #ifdef SOL_N20_SERVO_ETH_CONFIG
    {"open_sol_n2o", "Open N2O solenoid for specified duration (ms)", NULL, open_valve1, NULL, NULL, NULL},
    {"close_sol_n2o", "Close N2O solenoid", NULL, close_valve1, NULL, NULL, NULL},
    {"open_servo_eth", "Open ETH servo for specified duration (ms)", NULL, open_valve2, NULL, NULL, NULL},
    {"close_servo_eth", "Close ETH servo", NULL, close_valve2, NULL, NULL, NULL},
    {"auto_vent_on", "Activate auto vent", NULL, auto_vent_on, NULL, NULL, NULL},
    {"auto_vent_off", "Deactivate auto vent", NULL, auto_vent_off, NULL, NULL, NULL},
    {"get_auto_vent_data", "Get auto vent data", NULL, get_auto_vent_data, NULL, NULL, NULL},
    #elif defined(SOL_ETH_CONFIG)
    {"open_sol_eth", "Open ETH solenoid for specified duration (ms)", NULL, open_valve1, NULL, NULL, NULL},
    {"close_sol_eth", "Close ETH solenoid", NULL, close_valve1, NULL, NULL, NULL},
    #elif defined(SERVO_N20_CONFIG)
    {"open_servo_n2o", "Open N2O servo for specified duration (ms)", NULL, open_valve1, NULL, NULL, NULL},
    {"close_servo_n2o", "Close N2O servo", NULL, close_valve1, NULL, NULL, NULL},
    #elif defined(SOL_N2_CONFIG)
    {"open_sol_n2", "Open N2 solenoid for specified duration (ms)", NULL, open_valve1, NULL, NULL, NULL},
    {"close_sol_n2", "Close N2 solenoid", NULL, close_valve1, NULL, NULL, NULL},
    #else
    #error "No valve configuration defined!"
    #endif
};

// clang-format on

esp_err_t console_config_init() {
  esp_err_t ret;
  ret = console_init();
  ret = console_register_commands(cmd, sizeof(cmd) / sizeof(cmd[0]));
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "%s", esp_err_to_name(ret));
    return ret;
  }
  return ret;
}