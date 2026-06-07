#include <string.h>

#include "esp_log.h"
#include "esp_system.h"

#include "BoardData.h"
#include "auto_vent_task.h"
#include "board_config.h"
#include "buzzer.h"
#include "buzzer_task.h"
#include "commands.h"
#include "console.h"
#include "console_config.h"
#include "flash.h"
#include "i2c_scan.h"
#include "igniter_driver.h"
#include "ltc4162.h"
#include "now.h"
#include "pressure_driver.h"
#include "servo_config.h"
#include "valve_board_config.h"
#include "valves_control.h"

#define TAG "CONSOLE_CONFIG"

static int reset_device(int argc, char **argv) {
  ESP_LOGI(TAG, "Resetting device...");
  esp_restart();
  return 0;
}

static int run_i2c_scan(int argc, char **argv) {
  i2c_scan();
  return 0;
}

static int run_ltc4162_monitor(int argc, char **argv) {
  ltc4162_debug_monitor();
  return 0;
}

static int run_igniter_continuity_check(int argc, char **argv) {
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

static int run_igniter_arm(int argc, char **argv) {
  igniter_status_t status = igniter_arm(igniter_cfg);
  if (status != IGNITER_OK) {
    ESP_LOGE(TAG, "Igniter arming failed with status %d", status);
    return -1;
  }
  ESP_LOGI(TAG, "Igniter armed successfully");
  return 0;
}

static int run_igniter_disarm(int argc, char **argv) {
  igniter_status_t status = igniter_disarm(igniter_cfg);
  if (status != IGNITER_OK) {
    ESP_LOGE(TAG, "Igniter disarming failed with status %d", status);
    return -1;
  }
  ESP_LOGI(TAG, "Igniter disarmed successfully");
  return 0;
}

static int run_igniter_fire(int argc, char **argv) {
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

static int buzzer_play(int argc, char **argv) {
  if (argc < 2) {
    ESP_LOGE(TAG, "Usage: buzzer_play <sound_id>");
    ESP_LOGI(TAG, "Sound List:");
    ESP_LOGI(TAG, "1: SOUND_ALL_OK");
    ESP_LOGI(TAG, "2: SOUND_CHARGER_CONNECTED");
    ESP_LOGI(TAG, "3: SOUND_CHARGER_DISCONNECTED");
    ESP_LOGI(TAG, "4: SOUND_LOW_BATTERY");
    ESP_LOGI(TAG, "5: SOUND_INIT_ERROR");
    ESP_LOGI(TAG, "6: SOUND_SINGLE_BEEP");
    ESP_LOGI(TAG, "7: SOUND_DOUBLE_BEEP");
    ESP_LOGI(TAG, "8: SOUND_TRIPLE_BEEP");
    ESP_LOGI(TAG, "9: SOUND_QUADRUPLE_BEEP");
    ESP_LOGI(TAG, "10: SOUND_IMPERIAL_MARCH");
    ESP_LOGI(TAG, "11: SOUND_ODE_TO_JOY");
    ESP_LOGI(TAG, "12: SOUND_HARRY_POTTER_THEME");
    return -1;
  }
  sound_id_t sound_id = atoi(argv[1]);
  play_sound(sound_id);
  return 0;
}

static int open_valve1(int argc, char **argv) {
  if (argc < 2) {
    ESP_LOGE(TAG, "Usage: open_valve_name <duration_ms>");
    return -1;
  }

  int duration_ms = atoi(argv[1]);

#ifdef SOL_N20_SERVO_ETH_CONFIG
  handle_valve_cmd(N20_SOL_OPEN, duration_ms);
#elif defined(SOL_N2_CONFIG)
  handle_valve_cmd(N2_SOL_OPEN, duration_ms);
#elif defined(SERVO_N20_CONFIG)
  handle_valve_cmd(N20_VALVE_OPEN, duration_ms);
#elif defined(SOL_ETH_CONFIG)
  handle_valve_cmd(ETH_SOL_OPEN, duration_ms);
#else
  ESP_LOGE(TAG, "No valve configuration defined!");
#endif

  return 0;
}

static int close_valve1(int argc, char **argv) {

#ifdef SOL_N20_SERVO_ETH_CONFIG
  handle_valve_cmd(N20_SOL_CLOSE, 0);
#elif defined(SOL_N2_CONFIG)
  handle_valve_cmd(N2_SOL_CLOSE, 0);
#elif defined(SERVO_N20_CONFIG)
  handle_valve_cmd(N20_VALVE_CLOSE, 0);
#elif defined(SOL_ETH_CONFIG)
  handle_valve_cmd(ETH_SOL_CLOSE, 0);
#else
  ESP_LOGE(TAG, "No valve configuration defined!");
#endif
  return 0;
}

static int open_valve2(int argc, char **argv) {
  if (argc < 2) {
    ESP_LOGE(TAG, "Usage: open_valve_name <duration_ms>");
    return -1;
  }

  int duration_ms = atoi(argv[1]);

#ifdef SOL_N20_SERVO_ETH_CONFIG
  handle_valve_cmd(ETH_VALVE_OPEN, duration_ms);
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

static int close_valve2(int argc, char **argv) {

#ifdef SOL_N20_SERVO_ETH_CONFIG
  handle_valve_cmd(ETH_VALVE_CLOSE, 0);
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

static int set_now_send_log(int argc, char **argv) {
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

static int set_obc_test_data(int argc, char **argv) {
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

static int deinit_i2c(int argc, char **argv) {
  esp_err_t ret = mcu_i2c_deinit();
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "I2C deinitialization failed");
    return -1;
  }
  ESP_LOGI(TAG, "I2C deinitialized successfully");
  return 0;
}

static int init_i2c(int argc, char **argv) {
  esp_err_t ret = mcu_i2c_init();
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "I2C initialization failed");
    return -1;
  }
  ESP_LOGI(TAG, "I2C initialized successfully");
  return 0;
}

static int open_angle(int argc, char **argv) {
  if (argc < 3) {
    ESP_LOGE(TAG, "Usage: open_angle <valve_id> <angle>");
    return -1;
  }
  uint8_t valve_id = atoi(argv[1]);
  int angle = atoi(argv[2]);
  handle_valve_cmd_angle(valve_id, 0, angle);
  return 0;
}

#ifdef SOL_N20_SERVO_ETH_CONFIG
static int get_auto_vent_data(int argc, char **argv) {
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

static int print_bd_data(int argc, char **argv) {
  print_board_data();
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

// |--- Commands for Flash memory module ---|

void print_config(const data_config_t *cfg, const char *label) {
  printf("%s\n", label);
  flash_print_config(*cfg);
  printf("\n");
}

int read_flash(int argc, char **argv) {
  data_config_t data;
  if (flash_read(&data) != ESP_OK) {
    printf("Couldn't retrieve data from flash memory\n");
    return 0;
  }

  print_config(&data, "Memory contents:");
  return 0;
}

int get_runtime_config(int argc, char **argv) {
  data_config_t data;
  flash_get_runtime_config(&data);
  print_config(&data, "Runtime config:");
  return 0;
}

int save_flash(int argc, char **argv) {
  esp_err_t ret;
  ret = flash_commit();

  if (ret != ESP_OK) {
    printf("Couldn't save data to flash memory\nErr: %s\n",
           esp_err_to_name(ret));
    return 0;
  }

  printf("Successfully saved data to flash memory\n");
  apply_pressure_calibration();
  servo_apply_calibration();
  return 0;
}

int restore_defaults(int argc, char **argv) {
  esp_err_t ret;
  ret = flash_restore_defaults();

  if (ret != ESP_OK) {
    printf("Couldn't restore config default values\nErr: %s\n",
           esp_err_to_name(ret));
    return 0;
  }

  printf("Successfully restored config default values. Remember to use "
         "`save_config` to save your changes\n");
  return 0;
}

int erase_flash(int argc, char **argv) {
  if (strcmp(argv[1], "Y") != 0) {
    printf("Flash erase cancelled. You need to pass 'Y' as argument to "
           "confirm.\n");
    return 0;
  }

  esp_err_t ret = flash_erase_config();
  if (ret != ESP_OK) {
    printf("Couldn't erase flash contents\nErr: %s\n", esp_err_to_name(ret));
    return 0;
  }

  printf("Successfully erased flash contents\n");
  return 0;
}

int edit_flash(int argc, char **argv) {
  if (argc < 3) {
    printf("Usage: edit_flash <field> <value>\n");
    return 0;
  }

  const char *field = argv[1];
  const char *value = argv[2];

  esp_err_t ret = flash_edit_field(field, value);
  if (ret != ESP_OK) {
    printf("Couldn't edit provided field\nErr: %s\n", esp_err_to_name(ret));
    return 0;
  }

  printf("Successfully edited runtime config. Remember to use `save_config` to "
         "save your changes\n");
  return 0;
}

// |--- Commands for pressure sensors calibration ---|

const char *pressure_sensors_names[] = {
    "P2",
    "L",
    "S",
    "P1",

};

static esp_err_t parse_float(const char *value, float *out) {
  if (!value || !out)
    return ESP_ERR_INVALID_ARG;
  char *endptr = NULL;
  *out = strtof(value, &endptr);
  if (*endptr != '\0')
    return ESP_ERR_INVALID_ARG;
  return ESP_OK;
}

int press_tare(int argc, char **argv) {
  const char *field = NULL;
  int sensor_num = -1;
  if (argc > 1) {
    field = argv[1];

    size_t n =
        sizeof(pressure_sensors_names) / sizeof(pressure_sensors_names[0]);
    for (size_t i = 0; i < n; i++) {
      if (strcmp(pressure_sensors_names[i], field) == 0) {
        sensor_num = i;
        break;
      }
    }
  }

  data_config_t new_config;
  if (flash_get_runtime_config(&new_config) != ESP_OK) {
    printf("Couldn't retrieve runtime config\n");
    return 0;
  }

  float *config_fields[PRESSURE_DRIVER_SENSOR_COUNT] = {
      &new_config.press_calibr.sensor_0_volt_0,
      &new_config.press_calibr.sensor_1_volt_0,
      &new_config.press_calibr.sensor_2_volt_0,
      &new_config.press_calibr.sensor_3_volt_0,
  };

  if (sensor_num >= 0) {
    float mv;
    if (tare_pressure_sensor(&pressure_driver_config, sensor_num, &mv) !=
        PRESSURE_DRIVER_OK) {
      printf("Calibration failed while taring %s sensor.",
             pressure_sensors_names[sensor_num]);
      return 0;
    }
    *config_fields[sensor_num] = mv;
  } else {
    for (int i = 0; i < PRESSURE_DRIVER_SENSOR_COUNT; i++) {
      float mv;
      if (tare_pressure_sensor(&pressure_driver_config, i, &mv) !=
          PRESSURE_DRIVER_OK) {
        printf("Calibration failed while taring %s sensor.",
               pressure_sensors_names[i]);
        return 0;
      }
      *config_fields[i] = mv;
    }
  }

  if (flash_edit_config(new_config) != ESP_OK) {
    printf("Failed to save calibration values into runtime config\n");
    return 0;
  }

  if (sensor_num != -1) {
    printf("Successfully calibrated %s sensor for pressure of 0 bars. Remember "
           "to use `save_config` to save your changes\n",
           pressure_sensors_names[sensor_num]);
  } else {
    printf("Successfully calibrated all sensors for pressure of 0 bars. "
           "Remember to use `save_config` to save your changes\n");
  }
  return 0;
}

int press_calibrate(int argc, char **argv) {
  if (argc < 3) {
    printf("Usage: calibrate <sensor> <value>\n");

    size_t s_count =
        sizeof(pressure_sensors_names) / sizeof(pressure_sensors_names[0]);
    printf("Available sensors:\n");
    for (int i = 0; i < s_count; i++)
      printf("- %s\n", pressure_sensors_names[i]);

    return 0;
  }

  const char *field = argv[1];
  const char *value = argv[2];
  float pressure;

  if (parse_float(value, &pressure) != ESP_OK) {
    printf("Couldn't parse provided pressure\n");
    return 0;
  }

  data_config_t new_config;
  if (flash_get_runtime_config(&new_config) != ESP_OK) {
    printf("Couldn't retrieve runtime config\n");
    return 0;
  }

  struct {
    const char *key;
    float *ptrs[2];
  } sensor_map[] = {{pressure_sensors_names[0],
                     {&new_config.press_calibr.sensor_0_volt_1,
                      &new_config.press_calibr.sensor_0_press_1}},
                    {pressure_sensors_names[1],
                     {&new_config.press_calibr.sensor_1_volt_1,
                      &new_config.press_calibr.sensor_1_press_1}},
                    {pressure_sensors_names[2],
                     {&new_config.press_calibr.sensor_2_volt_1,
                      &new_config.press_calibr.sensor_2_press_1}},
                    {pressure_sensors_names[3],
                     {&new_config.press_calibr.sensor_3_volt_1,
                      &new_config.press_calibr.sensor_3_press_1}}};
  size_t n = sizeof(sensor_map) / sizeof(sensor_map[0]);

  float *voltage_1 = NULL, *pressure_1 = NULL;
  int sensor_num = 0;
  for (size_t i = 0; i < n; i++) {
    if (strcmp(sensor_map[i].key, field) == 0) {
      voltage_1 = sensor_map[i].ptrs[0];
      pressure_1 = sensor_map[i].ptrs[1];
      sensor_num = i;
      break;
    }
  }

  if (voltage_1 == NULL || pressure_1 == NULL) {
    printf("Couldn't parse provided field argument\n");
    return 0;
  }

  calibrate_pressure_sensor(&pressure_driver_config, sensor_num, pressure,
                            voltage_1);
  *pressure_1 = pressure;

  if (flash_edit_config(new_config) != ESP_OK) {
    printf("Failed to save calibration values into runtime config\n");
    return 0;
  }

  printf("Successfully calibrated %s sensor for pressure of %g bars. Remember "
         "to use `save_config` to save your changes\n",
         field, pressure);
  return 0;
}

int print_help(int argc, char **argv) {
  printf("\n=== %s console ===\n", CONFIG_NAME);

  printf("\n-- Data --\n");
  printf("  print_board_data     Print current board data\n");
  printf("  get_board_data       Same as print_board_data\n");
  printf("  now_send_log         ESP-NOW debug log (on|off|toggle)\n");
  printf(
      "  obc_test_data        Test data in ESP-NOW packets (on|off|toggle)\n");

  printf("\n-- Flash / config --\n");
  printf("  flash_read           Show data saved in flash memory\n");
  printf("  flash_display_config       Show runtime config (NOT flash "
         "contents)\n");
  printf("  flash_edit_config          <field> <value> - set field in runtime "
         "config\n");
  printf("  flash_save_config          Save runtime config to flash\n");
  printf("  flash_restore_config       Load defaults into runtime config\n");
  printf("  flash_erase          <Y> - erase config partition\n");
  printf("\n-- Pressure calibration --\n");
  printf(
      "  tare                 [sensor] - calibrate for 0 bar (all if empty)\n");
  printf(
      "  calibrate            <sensor> <bar> - calibrate for given pressure\n");
  printf("                       sensors: P1, P2, L, S\n");

  printf("\n-- Debug --\n");
  printf("  i2c_scan             Scan the I2C bus for devices\n");
  printf("  init_i2c             Initialize the I2C bus\n");
  printf("  deinit_i2c           Deinitialize the I2C bus\n");

  printf("\n-- System --\n");
  printf("  help                 Show this help\n");
  printf("  reset                Reset the device\n");
  printf("  ltc_monitor          Run LTC4162 debug monitor\n");
  printf("  buzzer_play          Play a sound on the buzzer\n");

  printf("\n-- Valves --\n");
  printf(
      "  open_angle           <angle> - open a valve to a specified angle\n");
#ifdef SOL_N20_SERVO_ETH_CONFIG
  printf("  open_sol_n2o         <ms> - open N2O solenoid\n");
  printf("  close_sol_n2o        Close N2O solenoid\n");
  printf("  open_servo_eth       <ms> - open ETH servo\n");
  printf("  close_servo_eth      Close ETH servo\n");
  printf("  auto_vent_on         <bar> - activate auto vent\n");
  printf("  auto_vent_off        Deactivate auto vent\n");
  printf("  get_auto_vent_data   Get auto vent data\n");
#elif defined(SOL_ETH_CONFIG)
  printf("  open_sol_eth         <ms> - open ETH solenoid\n");
  printf("  close_sol_eth        Close ETH solenoid\n");
#elif defined(SERVO_N20_CONFIG)
  printf("  open_servo_n2o       <ms> - open N2O servo\n");
  printf("  close_servo_n2o      Close N2O servo\n");
#elif defined(SOL_N2_CONFIG)
  printf("  open_sol_n2          <ms> - open N2 solenoid\n");
  printf("  close_sol_n2         Close N2 solenoid\n");
#endif

  printf("\n");
  return 0;
}

// Place for the console configuration

// clang-format off
static esp_console_cmd_t cmd[] = {
    // example command:
    // cmd     help description   hint  function      args  func_context context
    {"reset", "Reset the device", NULL, reset_device, NULL, NULL, NULL},
    {"i2c_scan", "Scan the I2C bus for devices", NULL, run_i2c_scan, NULL, NULL, NULL},
    {"ltc_monitor", "Run LTC4162 debug monitor", NULL, run_ltc4162_monitor, NULL, NULL, NULL},
    // {"igniter_continuity", "Check igniter continuity", NULL, run_igniter_continuity_check, NULL, NULL, NULL},
    // {"igniter_arm", "Arm the igniter", NULL, run_igniter_arm, NULL, NULL, NULL},
    // {"igniter_disarm", "Disarm the igniter", NULL, run_igniter_disarm, NULL, NULL, NULL},
    // {"igniter_fire", "Fire the igniter", NULL, run_igniter_fire, NULL, NULL, NULL},
    {"buzzer_play", "Play a sound on the buzzer", NULL, buzzer_play, NULL, NULL, NULL},
    {"now_send_log", "Enable/disable ESP-NOW data-to-OBC debug log (on|off or toggle)", NULL, set_now_send_log, NULL, NULL, NULL},
    {"obc_test_data", "Enable/disable test data in ESP-NOW packets to OBC (on|off or toggle)", NULL, set_obc_test_data, NULL, NULL, NULL},
    {"deinit_i2c", "Deinitialize the I2C bus", NULL, deinit_i2c, NULL, NULL, NULL},
    {"init_i2c", "Initialize the I2C bus", NULL, init_i2c, NULL, NULL, NULL},
    {"open_angle", "Open a valve to a specified angle", NULL, open_angle, NULL, NULL, NULL},
    {"print_board_data", "Print current board data to console", NULL, print_bd_data, NULL, NULL, NULL},

    {"flash_read", "Reads and displays saved data in flash memory.", NULL, read_flash, NULL, NULL, NULL},
    {"flash_display_config", "Displays current state of runtime config.\nThis command does NOT display flash memory contents, to see current flash memory contents use `read_flash`.", NULL, get_runtime_config, NULL, NULL, NULL},
    {"flash_save_config", "Saves runtime config edited by User to flash memory.", NULL, save_flash, NULL, NULL, NULL},
    {"flash_edit_config", "Sets the provided field in runtime config to provided value.", NULL, edit_flash, NULL, NULL, NULL},
    {"flash_restore_config", "Restores all default values and saves them into runtime config.\nUse `save_config` to save the runtime config to flash memory.", NULL, restore_defaults, NULL, NULL, NULL},
    {"flash_erase", "Erases flash memory partition that is holding config data.\nTo erase stored data you need to type `erase_flash Y` to ensure that flash won't be erased by accident.\nThere is no need to run `save_flash` after this function finishes.", NULL, erase_flash, NULL, NULL, NULL},

    {"calibrate", "Calibrate specific sensor for provided pressure", NULL, press_calibrate, NULL, NULL, NULL},
    {"tare", "Calibrate all or chosen pressure sensors for 0 bar", NULL, press_tare, NULL, NULL, NULL},

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
    {"help", "Show this help", NULL, print_help, NULL, NULL, NULL},
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