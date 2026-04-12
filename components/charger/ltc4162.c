#include "ltc4162.h"
#include "esp_log.h"
#include "mcu_i2c_config.h"
#include <stdint.h>
#include <stdio.h>

#define TAG "LTC4162"


esp_err_t ltc4162_read_register(uint8_t reg, uint8_t *data, size_t len) {
  if (len == 0 || data == NULL) {
    return ESP_ERR_INVALID_ARG;
  }
  return _mcu_i2c_read(LTC4162_I2C_ADDRESS, reg, data, len) ? ESP_OK : ESP_FAIL;
}

esp_err_t ltc4162_write_register(uint8_t reg, uint8_t *data, size_t len) {
  if (len == 0 || data == NULL) {
    return ESP_ERR_INVALID_ARG;
  }
  return _mcu_i2c_write(LTC4162_I2C_ADDRESS, reg, data, len) ? ESP_OK
                                                             : ESP_FAIL;
}

esp_err_t ltc4162_init(void) {
  uint16_t config = (1 << 2); // force_telemetry_on
  if (ltc4162_write_register(0x14, (uint8_t *)&config, 2) != ESP_OK) {
    ESP_LOGE(TAG, "Failed to write config register");
    return ESP_FAIL;
  }

  uint16_t config_check = 0;

  if (ltc4162_read_register(0x14, (uint8_t *)&config_check, 2) != ESP_OK) {
    ESP_LOGE(TAG, "Failed to read back config register");
    return ESP_FAIL;
  }

  // ESP_LOGI(TAG, "Force telemetry on: %s",
          //  (config_check & (1 << 2)) ? "ON" : "OFF");
  return ESP_OK;
}

#define RSNSI 0.01f // przykład: 10mΩ
#define RSNSB 0.01f // przykład: 10mΩ

static int16_t read_reg16(uint8_t reg) {
  uint8_t buf[2] = {0};
  if (ltc4162_read_register(reg, buf, 2) != ESP_OK) {
    ESP_LOGE(TAG, "Read failed: 0x%02X", reg);
    return 0;
  }

  // little endian
  return (int16_t)((buf[1] << 8) | buf[0]);
}

static esp_err_t read_reg16_checked(uint8_t reg, int16_t *value) {
  uint8_t buf[2] = {0};
  if (value == NULL) {
    return ESP_ERR_INVALID_ARG;
  }

  if (ltc4162_read_register(reg, buf, 2) != ESP_OK) {
    ESP_LOGE(TAG, "Read failed: 0x%02X", reg);
    return ESP_FAIL;
  }

  *value = (int16_t)((buf[1] << 8) | buf[0]);
  return ESP_OK;
}

esp_err_t read_charger_data(ltc4162_charger_data_t *charger_data) {
  if (charger_data == NULL) {
    return ESP_ERR_INVALID_ARG;
  }

  if(ltc4162_init() != ESP_OK) {
    ESP_LOGE(TAG, "Failed to init LTC4162");
    return ESP_FAIL;
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

  if (read_reg16_checked(0x3A, &vbat_raw) != ESP_OK ||
      read_reg16_checked(0x3B, &vin_raw) != ESP_OK ||
      read_reg16_checked(0x3C, &vout_raw) != ESP_OK ||
      read_reg16_checked(0x3D, &iin_raw) != ESP_OK ||
      read_reg16_checked(0x3E, &ibat_raw) != ESP_OK ||
      read_reg16_checked(0x3F, &die_raw) != ESP_OK ||
      read_reg16_checked(0x39, &sys_raw) != ESP_OK ||
      read_reg16_checked(0x34, &charger_state_raw) != ESP_OK ||
      read_reg16_checked(0x43, &chem_cells) != ESP_OK) {
    return ESP_FAIL;
  }

  uint8_t cell_count = chem_cells & 0x0F;
  if (cell_count == 0) {
    cell_count = 4;
  }

  charger_data->vbat = vbat_raw * (cell_count * 192.4e-6f);
  charger_data->vin = vin_raw * 1.649e-3f;
  charger_data->vout = vout_raw * 1.653e-3f;
  charger_data->iin = (iin_raw * 1.466e-6f) / RSNSI;
  charger_data->ibat = (ibat_raw * 1.466e-6f) / RSNSB;
  charger_data->die_temp = die_raw * 0.0215f - 264.4f;
  charger_data->charger_status = (sys_raw & (1 << 8)) != 0;
  charger_data->charger_state = charger_state_raw == 2 ? 1 : 0;

  return ESP_OK;
}

esp_err_t ltc4162_debug_monitor(void) {
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

  /* ---------- CHARGER STATE ---------- */

  int16_t charger_state = read_reg16(0x34);
  int16_t charge_status = read_reg16(0x35);

  ESP_LOGI(TAG, "Charger state: 0x%04X", charger_state);
  ESP_LOGI(TAG, "Charge status: 0x%04X", charge_status);

  if (charger_state == 64)
    ESP_LOGI(TAG, "State: CC/CV Charge");
  else if (charger_state == 128)
    ESP_LOGI(TAG, "State: Precharge");
  else if (charger_state == 256)
    ESP_LOGI(TAG, "State: Suspended");

  if (charge_status & 0x02)
    ESP_LOGI(TAG, "Mode: Constant Current");
  if (charge_status & 0x01)
    ESP_LOGI(TAG, "Mode: Constant Voltage");

  ESP_LOGI(TAG, "===== END DEBUG =====");

  return ESP_OK;
}

