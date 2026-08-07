#include "ltc4162.h"
#include "esp_log.h"
#include <stdint.h>
#include <stdio.h>

#define TAG "LTC4162"

static ltc4162_config_t LTC_config = {0};
static bool LTC_initialized = false;

#define RSNSI 0.01f // przykład: 10mΩ
#define RSNSB 0.01f // przykład: 10mΩ

// |--- LTC REGISTERS ---|

#define LTC4162_REG_CONFIG_BITS 0x14
#define LTC4162_REG_EN_LIMIT_ALERTS 0x0D
#define LTC4162_REG_EN_CHARGER_STATE_ALERTS 0x0E
#define LTC4162_REG_EN_CHARGE_STATUS_ALERTS 0x0F
#define LTC4162_REG_CHARGER_STATE 0x34
#define LTC4162_REG_CHARGE_STATUS 0x35
#define LTC4162_REG_LIMIT_ALERTS 0x36
#define LTC4162_REG_CHARGER_STATE_ALERTS 0x37
#define LTC4162_REG_CHARGE_STATUS_ALERTS 0x38

// |--- I2C FUNCTIONS ---|

esp_err_t ltc4162_read_register(uint8_t reg, uint8_t *data, uint8_t len) {
  if (LTC_config.i2c_read == NULL)
    return ESP_ERR_INVALID_STATE;
  if (len == 0 || data == NULL)
    return ESP_ERR_INVALID_ARG;

  return LTC_config.i2c_read(LTC_config.i2c_address, reg, data, len) ? ESP_OK
                                                                     : ESP_FAIL;
}

esp_err_t ltc4162_write_register(uint8_t reg, uint8_t *data, uint8_t len) {
  if (LTC_config.i2c_write == NULL)
    return ESP_ERR_INVALID_STATE;
  if (len == 0 || data == NULL)
    return ESP_ERR_INVALID_ARG;

  return LTC_config.i2c_write(LTC_config.i2c_address, reg, data, len)
             ? ESP_OK
             : ESP_FAIL;
}

// |--- HELPER FUNCTIONS ---|

static int16_t read_reg16(uint8_t reg) {
  uint8_t buf[2] = {0};
  if (ltc4162_read_register(reg, buf, 2) != ESP_OK) {
    ESP_LOGE(TAG, "Read failed (unchecked): 0x%02X", reg);
    return 0;
  }

  return (int16_t)((uint16_t)buf[0] | ((uint16_t)buf[1] << 8));
}

static esp_err_t read_reg16_checked(uint8_t reg, int16_t *value) {
  if (value == NULL)
    return ESP_ERR_INVALID_ARG;

  uint8_t buf[2] = {0};
  if (ltc4162_read_register(reg, buf, 2) != ESP_OK) {
    ESP_LOGE(TAG, "Read failed (checked): 0x%02X", reg);
    return ESP_FAIL;
  }

  *value = (int16_t)((uint16_t)buf[0] | ((uint16_t)buf[1] << 8));
  return ESP_OK;
}

static esp_err_t write_reg16(uint8_t reg, uint16_t value) {
  uint8_t buf[2];

  buf[0] = value & 0xFF;        // LSB
  buf[1] = (value >> 8) & 0xFF; // MSB

  return ltc4162_write_register(reg, buf, 2);
}

// |--- PUBLIC FUNCTIONS ---|

esp_err_t ltc4162_init(const ltc4162_config_t *config) {
  if (config == NULL) {
    ESP_LOGE(TAG, "Config pointer is NULL");
    return ESP_ERR_INVALID_ARG;
  }
  if (config->i2c_read == NULL || config->i2c_write == NULL) {
    ESP_LOGE(TAG, "I2C read/write functions must be provided");
    return ESP_ERR_INVALID_ARG;
  }

  LTC_config = *config;

  uint16_t telemetry_cmd = (1 << 2); // force_telemetry_on
  if (write_reg16(LTC4162_REG_CONFIG_BITS, telemetry_cmd) != ESP_OK) {
    ESP_LOGE(
        TAG,
        "Hardware communication error: Failed to write config register 0x%02X",
        LTC4162_REG_CONFIG_BITS);
    return ESP_FAIL;
  }

  int16_t config_check = 0;
  if (read_reg16_checked(LTC4162_REG_CONFIG_BITS, &config_check) != ESP_OK) {
    ESP_LOGE(TAG,
             "Hardware communication error: Failed to read back config "
             "register 0x%02X",
             LTC4162_REG_CONFIG_BITS);
    return ESP_FAIL;
  }
  if ((config_check & (1 << 2)) == 0) {
    ESP_LOGE(TAG, "Verification failed: force_telemetry_on bit did not clear "
                  "or set correctly");
    return ESP_FAIL;
  }

  if (LTC_config.int_pin != GPIO_NUM_NC && LTC_config.int_pin >= 0) {
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << LTC_config.int_pin),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE // There is physical PULLUP on board
    };

    esp_err_t err = gpio_config(&io_conf);
    if (err != ESP_OK) {
      ESP_LOGW(TAG,
               "INT pin configuration failed (code: %s), continuing without "
               "hardware interrupts",
               esp_err_to_name(err));
    }
  }

  LTC_initialized = true;
  ESP_LOGI(TAG, "LTC4162 initialized successfully at address 0x%02X",
           LTC_config.i2c_address);
  return ESP_OK;
}

esp_err_t ltc4162_deinit(void) {
  if (!LTC_initialized)
    return ESP_ERR_INVALID_STATE;

  if (ltc4162_enable_interrupts(LTC4162_ALERT_LIMIT_NONE,
                                LTC4162_ALERT_STATE_NONE,
                                LTC4162_ALERT_STATUS_NONE) != ESP_OK) {
    ESP_LOGW(TAG, "Could not clear hardware alert masks (I2C error), forcing "
                  "software deinit");
  }

  LTC_initialized = false;
  LTC_config = (ltc4162_config_t){0};

  ESP_LOGI(TAG, "LTC4162 driver deinitialized");
  return ESP_OK;
}

esp_err_t ltc4162_enable_interrupts(uint16_t limit_mask, uint16_t state_mask,
                                    uint16_t status_mask) {
  if (!LTC_initialized)
    return ESP_ERR_INVALID_STATE;

  if (write_reg16(LTC4162_REG_EN_LIMIT_ALERTS, limit_mask) != ESP_OK)
    return ESP_FAIL;
  if (write_reg16(LTC4162_REG_EN_CHARGER_STATE_ALERTS, state_mask) != ESP_OK)
    return ESP_FAIL;
  if (write_reg16(LTC4162_REG_EN_CHARGE_STATUS_ALERTS, status_mask) != ESP_OK)
    return ESP_FAIL;

  return ESP_OK;
}

esp_err_t ltc4162_get_alerts(uint16_t *limit_alerts, uint16_t *state_alerts,
                             uint16_t *status_alerts) {
  if (!LTC_initialized)
    return ESP_ERR_INVALID_STATE;
  if (!limit_alerts || !state_alerts || !status_alerts)
    return ESP_ERR_INVALID_ARG;

  int16_t raw_limit = 0, raw_state = 0, raw_status = 0;

  if (read_reg16_checked(LTC4162_REG_LIMIT_ALERTS, &raw_limit) != ESP_OK)
    return ESP_FAIL;
  if (read_reg16_checked(LTC4162_REG_CHARGER_STATE_ALERTS, &raw_state) !=
      ESP_OK)
    return ESP_FAIL;
  if (read_reg16_checked(LTC4162_REG_CHARGE_STATUS_ALERTS, &raw_status) !=
      ESP_OK)
    return ESP_FAIL;

  *limit_alerts = (uint16_t)raw_limit;
  *state_alerts = (uint16_t)raw_state;
  *status_alerts = (uint16_t)raw_status;

  return ESP_OK;
}

esp_err_t ltc4162_set_suspend(bool suspend) {
  if (!LTC_initialized)
    return ESP_ERR_INVALID_STATE;

  int16_t current_config = 0;
  if (read_reg16_checked(LTC4162_REG_CONFIG_BITS, &current_config) != ESP_OK)
    return ESP_FAIL;

  if (suspend)
    current_config |= (1 << 5);
  else
    current_config &= ~(1 << 5);

  if (write_reg16(LTC4162_REG_CONFIG_BITS, (uint16_t)current_config) !=
      ESP_OK) {
    return ESP_FAIL;
  }

  ESP_LOGI(TAG, "Charger status updated: %s",
           suspend ? "SUSPENDED" : "RESUMED");
  return ESP_OK;
}

esp_err_t read_charger_data(ltc4162_charger_data_t *charger_data) {
  if (charger_data == NULL)
    return ESP_ERR_INVALID_ARG;
  if (!LTC_initialized) {
    //   ESP_LOGE(TAG, "LTC4162 not initialized");
    return ESP_ERR_INVALID_STATE;
  }

  int16_t vbat_raw = 0;
  int16_t vin_raw = 0;
  int16_t vout_raw = 0;
  int16_t iin_raw = 0;
  int16_t ibat_raw = 0;
  int16_t die_raw = 0;
  int16_t sys_raw = 0;
  int16_t charger_state_raw = 0;
  int16_t chem_cells = 0;
  int16_t system_status_raw = 0;

  if (read_reg16_checked(0x3A, &vbat_raw) != ESP_OK)
    return ESP_FAIL;
  if (read_reg16_checked(0x3B, &vin_raw) != ESP_OK)
    return ESP_FAIL;
  if (read_reg16_checked(0x3C, &vout_raw) != ESP_OK)
    return ESP_FAIL;
  if (read_reg16_checked(0x3D, &iin_raw) != ESP_OK)
    return ESP_FAIL;
  if (read_reg16_checked(0x3E, &ibat_raw) != ESP_OK)
    return ESP_FAIL;
  if (read_reg16_checked(0x3F, &die_raw) != ESP_OK)
    return ESP_FAIL;
  if (read_reg16_checked(0x39, &sys_raw) != ESP_OK)
    return ESP_FAIL;
  if (read_reg16_checked(0x34, &charger_state_raw) != ESP_OK)
    return ESP_FAIL;
  if (read_reg16_checked(0x43, &chem_cells) != ESP_OK)
    return ESP_FAIL;
  if (read_reg16_checked(0x39, &system_status_raw) != ESP_OK)
    return ESP_FAIL;

  uint8_t cell_count = chem_cells & 0x0F;
  if (cell_count == 0)
    cell_count = 4;

  charger_data->vbat =
      vbat_raw * (cell_count * 192.4e-6f);   // Battery voltage (V)
  charger_data->vin = vin_raw * 1.649e-3f;   // Input voltage (V)
  charger_data->vout = vout_raw * 1.653e-3f; // System output voltage (V)
  charger_data->iin = (iin_raw * 1.466e-6f) / RSNSI;   // Input current (A)
  charger_data->ibat = (ibat_raw * 1.466e-6f) / RSNSB; // Battery current (A)
  charger_data->die_temp =
      die_raw * 0.0215f - 264.4f; // IC die temperature (°C)
  charger_data->charger_status =
      (sys_raw & (1 << 8)) != 0; // If charger is actively power pathing/running
  charger_data->charger_state =
      (charger_state_raw == 2) ? 1 : 0; // 1 for active charging, 0 otherwise
  charger_data->system_status = system_status_raw;
  charger_data->vout = vout_raw * 1.653e-3f;
  return ESP_OK;
}

esp_err_t ltc4162_debug_monitor(void) {
  if (!LTC_initialized) {
    ESP_LOGE(TAG, "LTC4162 not initialized");
    return ESP_ERR_INVALID_STATE;
  }

  ESP_LOGI(TAG, "===== LTC4162 DEBUG MONITOR =====");

  /* ---------- ALERT LIMITS ---------- */
  ESP_LOGI(TAG, "Alert limits:");

  int16_t vbat_lo = read_reg16(0x01);
  int16_t vbat_hi = read_reg16(0x02);
  int16_t vin_lo = read_reg16(0x03);
  int16_t vin_hi = read_reg16(0x04);
  int16_t vout_lo = read_reg16(0x05);
  int16_t vout_hi = read_reg16(0x06);
  int16_t iin_hi = read_reg16(0x07);
  int16_t ibat_lo = read_reg16(0x08);
  int16_t die_hi = read_reg16(0x09);
  int16_t bsr_hi = read_reg16(0x0A);

  ESP_LOGI(TAG, "VBAT LO raw: %d", vbat_lo);
  ESP_LOGI(TAG, "VBAT HI raw: %d", vbat_hi);
  ESP_LOGI(TAG, "VIN LO raw:  %d", vin_lo);
  ESP_LOGI(TAG, "VIN HI raw:  %d", vin_hi);
  ESP_LOGI(TAG, "VOUT LO raw: %d", vout_lo);
  ESP_LOGI(TAG, "VOUT HI raw: %d", vout_hi);
  ESP_LOGI(TAG, "IIN HI raw:  %d", iin_hi);
  ESP_LOGI(TAG, "IBAT LO raw: %d", ibat_lo);
  ESP_LOGI(TAG, "DIE TEMP HI raw: %d", die_hi);
  ESP_LOGI(TAG, "BSR HI raw: %d", bsr_hi);

  /* ---------- TELEMETRY ---------- */

  int16_t vbat_raw = read_reg16(0x3A);
  int16_t vin_raw = read_reg16(0x3B);
  int16_t vout_raw = read_reg16(0x3C);

  int16_t iin_raw = read_reg16(0x3D);  // jeśli używasz
  int16_t ibat_raw = read_reg16(0x3E); // jeśli używasz
  int16_t die_raw = read_reg16(0x3F);  // jeśli używasz

  /* ---------- CHEM + CELL COUNT ---------- */

  int16_t chem_cells = read_reg16(0x43);
  uint8_t cell_count = chem_cells & 0x0F;
  uint8_t chem = (chem_cells >> 8) & 0x0F;

  ESP_LOGI(TAG, "Cell count: %u", cell_count);
  ESP_LOGI(TAG, "Chemistry enum: %u", chem);

  /* ---------- KONWERSJE FIZYCZNE ---------- */

  float vbat = vbat_raw * (cell_count * 192.4e-6f);
  float vin = vin_raw * 1.649e-3f;
  float vout = vout_raw * 1.653e-3f;
  float iin = (iin_raw * 1.466e-6f) / RSNSI;
  float ibat = (ibat_raw * 1.466e-6f) / RSNSB;
  float die_temp = die_raw * 0.0215f - 264.4f;

  ESP_LOGI(TAG, "VBAT: %.3f V", vbat);
  ESP_LOGI(TAG, "VIN : %.3f V", vin);
  ESP_LOGI(TAG, "VOUT: %.3f V", vout);
  ESP_LOGI(TAG, "IIN : %.3f A", iin);
  ESP_LOGI(TAG, "IBAT: %.3f A", ibat);
  ESP_LOGI(TAG, "DIE TEMP: %.2f C", die_temp);

  /* ---------- SYSTEM STATUS ---------- */

  int16_t sys = read_reg16(0x39);

  ESP_LOGI(TAG, "System Status raw: 0x%04X", sys);

  if (sys & (1 << 8))
    ESP_LOGW(TAG, "Charging active");
  if (sys & (1 << 7))
    ESP_LOGE(TAG, "Cell count error");
  if (sys & (1 << 5))
    ESP_LOGW(TAG, "No RT resistor");
  if (sys & (1 << 4))
    ESP_LOGE(TAG, "Thermal shutdown!");
  if (sys & (1 << 3))
    ESP_LOGE(TAG, "VIN overvoltage!");
  if (sys & (1 << 2))
    ESP_LOGI(TAG, "VIN > VBAT");
  if (sys & (1 << 1))
    ESP_LOGI(TAG, "VIN > 4.2V");
  if (sys & (1 << 0))
    ESP_LOGI(TAG, "INTVCC OK");

  /* ---------- CHARGER STATE & STATUS---------- */

  int16_t charger_state = read_reg16(LTC4162_REG_CHARGER_STATE);
  int16_t charge_status = read_reg16(LTC4162_REG_CHARGE_STATUS);

  ESP_LOGI(
      TAG, "Charger state: 0x%04X (%s)", charger_state,
      ltc4162_charger_state_to_str((ltc4162_charger_state_t)charger_state));
  ESP_LOGI(
      TAG, "Charge status: 0x%04X (%s)", charge_status,
      ltc4162_charge_status_to_str((ltc4162_charge_status_t)charge_status));

  ESP_LOGI(TAG, "===== END DEBUG =====");
  return ESP_OK;
}

// |--- STRING CONVERSION UTILITIES ---|

const char *ltc4162_charger_state_to_str(ltc4162_charger_state_t state) {
  // Individual bits in charger state register are mutually exclusive
  switch (state) {
  case LTC4162_CHARGER_STATE_NONE:
    return "None";
  case LTC4162_CHARGER_BAT_DETECT_FAILED:
    return "Battery Detection Failed";
  case LTC4162_CHARGER_BATTERY_DETECTION:
    return "Battery Detection";
  case LTC4162_CHARGER_SUSPENDED:
    return "Suspended";
  case LTC4162_CHARGER_PRECHARGE:
    return "Precharge";
  case LTC4162_CHARGER_CC_CV_CHARGE:
    return "CC/CV Charge";
  case LTC4162_CHARGER_NTC_PAUSE:
    return "NTC Pause";
  case LTC4162_CHARGER_TIMER_TERMINATION:
    return "Timer Termination";
  case LTC4162_CHARGER_C_OVER_X_TERMINATION:
    return "C/X Termination";
  case LTC4162_CHARGER_MAX_CHARGE_TIME:
    return "Max Charge Time Fault";
  case LTC4162_CHARGER_BAT_MISSING_FAULT:
    return "Battery Missing Fault";
  case LTC4162_CHARGER_BAT_SHORT_FAULT:
    return "Battery Short Fault";
  default:
    return "Unknown State";
  }
}

const char *ltc4162_charge_status_to_str(ltc4162_charge_status_t status) {
  // Individual bits in charge status register are mutually exclusive
  switch (status) {
  case LTC4162_CHARGE_STATUS_OFF:
    return "Charger Off";
  case LTC4162_CHARGE_STATUS_CONSTANT_VOLTAGE:
    return "Constant Voltage";
  case LTC4162_CHARGE_STATUS_CONSTANT_CURRENT:
    return "Constant Current";
  case LTC4162_CHARGE_STATUS_IIN_LIMIT_ACTIVE:
    return "Input Current Limit Active";
  case LTC4162_CHARGE_STATUS_VIN_UVCL_ACTIVE:
    return "Input UVCL Active";
  case LTC4162_CHARGE_STATUS_THERMAL_REG_ACTIVE:
    return "Thermal Regulation Active";
  case LTC4162_CHARGE_STATUS_ILIM_REG_ACTIVE:
    return "Output Current Limit Active";
  default:
    return "Unknown Status";
  }
}
