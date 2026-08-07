#ifndef ltc4162_h
#define ltc4162_h

#include "esp_err.h"
#include "mcu_i2c_config.h"
#include "stdbool.h"

#define LTC4162_I2C_ADDRESS 0x68
#define LTC4162_DEFAULT_CONFIG(INT_PIN)                                        \
  (ltc4162_config_t) {                                                         \
    .i2c_address = LTC4162_I2C_ADDRESS, .i2c_read = _mcu_i2c_read,             \
    .i2c_write = _mcu_i2c_write, .int_pin = INT_PIN                            \
  }

typedef bool (*i2c_read_fn)(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data,
                            uint8_t len);
typedef bool (*i2c_write_fn)(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data,
                             uint8_t len);

typedef struct {
  uint8_t i2c_address;
  i2c_read_fn i2c_read;
  i2c_write_fn i2c_write;
  gpio_num_t int_pin;
} ltc4162_config_t;

typedef enum {
  LTC4162_ALERT_LIMIT_NONE = 0x0000,
  LTC4162_ALERT_TELEMETRY_VALID =
      (1
       << 15), // Telemetry system initialization complete and ADC data is valid
  LTC4162_ALERT_BSR_DONE = (1 << 14), // Battery internal resistance measurement
                                      // cycle (BSR) has completed
  LTC4162_ALERT_VBAT_LO =
      (1 << 11), // Battery voltage fell below the low threshold limit
  LTC4162_ALERT_VBAT_HI =
      (1 << 10), // Battery voltage exceeded the high threshold limit
  LTC4162_ALERT_VIN_LO =
      (1 << 9), // Input supply voltage fell below the low threshold limit
  LTC4162_ALERT_VIN_HI =
      (1 << 8), // Input supply voltage exceeded the high threshold limit
  LTC4162_ALERT_VOUT_LO =
      (1
       << 7), // System output voltage (VOUT) fell below the low threshold limit
  LTC4162_ALERT_VOUT_HI =
      (1
       << 6), // System output voltage (VOUT) exceeded the high threshold limit
  LTC4162_ALERT_IIN_HI = (1 << 5),      // Input current drawn from the supply
                                        // exceeded the high threshold limit
  LTC4162_ALERT_IBAT_LO = (1 << 4),     // Battery current fell below the low
                                        // threshold limit (deep discharge)
  LTC4162_ALERT_DIE_TEMP_HI = (1 << 3), // LTC4162 internal silicon die
                                        // temperature exceeded the high limit
  LTC4162_ALERT_BSR_HI = (1 << 2), // Battery internal resistance exceeded the
                                   // high limit (indicates aged/worn battery)
  LTC4162_ALERT_THERMISTOR_HI =
      (1 << 1), // Thermistor voltage high (indicates that the battery
                // temperature is too cold)
  LTC4162_ALERT_THERMISTOR_LO =
      (1 << 0) // Thermistor voltage low (indicates that the battery temperature
               // is too hot)
} ltc4162_alert_limit_t;

typedef enum {
  LTC4162_ALERT_STATE_NONE = 0x0000,
  LTC4162_ALERT_BAT_DETECT_FAILED =
      (1 << 12), // Battery detection test failed to determine battery presence
  LTC4162_ALERT_BATTERY_DETECTION =
      (1
       << 11), // Charger has initiated the battery presence detection sequence
  LTC4162_ALERT_CHARGER_SUSPENDED =
      (1 << 8), // Charging has been suspended programmatically or due to safety
                // triggers
  LTC4162_ALERT_PRECHARGE =
      (1 << 7), // Pre-charge phase has started (low-current charging for deeply
                // discharged cells)
  LTC4162_ALERT_CC_CV_CHARGE =
      (1 << 6), // Charger transitioned into Constant-Current (CC) or
                // Constant-Voltage (CV) mode
  LTC4162_ALERT_NTC_PAUSE =
      (1 << 5), // Battery temperature is outside the safe JEITA profile range
  LTC4162_ALERT_TIMER_TERMINATION =
      (1 << 4), // Maximum allowable safety time for CV phase elapsed
  LTC4162_ALERT_C_OVER_X_TERMINATION =
      (1 << 3), // Charge current fell below the full-charge threshold
  LTC4162_ALERT_MAX_CHARGE_TIME =
      (1 << 2), // Maximum total safety charge time has been exceeded
  LTC4162_ALERT_BAT_MISSING_FAULT =
      (1 << 1), // No battery detected while trying to initiate a charge cycle
  LTC4162_ALERT_BAT_SHORT_FAULT =
      (1 << 0) // A shorted cell or short-circuit condition was detected on the
               // battery
} ltc4162_alert_state_t;

typedef enum {
  LTC4162_ALERT_STATUS_NONE = 0x0000,
  LTC4162_ALERT_VIN_UVCL_ACTIVE =
      (1 << 3), // Input Under-Voltage Charge Limit loop is actively throttling
                // current due to weak input source
  LTC4162_ALERT_IIN_LIMIT_ACTIVE =
      (1 << 2), // Input current limit reached; charge current is reduced to
                // prioritize system load
  LTC4162_ALERT_THERMAL_REG_ACTIVE =
      (1 << 1), // Die temperature reached 115°C; hardware is reducing charge
                // current to prevent overheating
  LTC4162_ALERT_PROG_I_LIMIT_ACTIVE =
      (1 << 0) // Main programmed charge current regulation loop (I-charge loop)
               // has become active
} ltc4162_alert_status_t;

typedef enum {
  LTC4162_CHARGER_STATE_NONE = 0x0000,
  LTC4162_CHARGER_BAT_DETECT_FAILED =
      (1 << 12), // Battery detection test failed to determine battery presence
  LTC4162_CHARGER_BATTERY_DETECTION =
      (1
       << 11), // Charger has initiated the battery presence detection sequence
  LTC4162_CHARGER_SUSPENDED =
      (1 << 8), // Charging has been suspended programmatically or due to safety
                // triggers
  LTC4162_CHARGER_PRECHARGE =
      (1 << 7), // Pre-charge phase has started (low-current charging for deeply
                // discharged cells)
  LTC4162_CHARGER_CC_CV_CHARGE =
      (1 << 6), // Charger transitioned into Constant-Current (CC) or
                // Constant-Voltage (CV) mode
  LTC4162_CHARGER_NTC_PAUSE =
      (1 << 5), // Battery temperature is outside the safe JEITA profile range
  LTC4162_CHARGER_TIMER_TERMINATION =
      (1 << 4), // Maximum allowable safety time for CV phase elapsed
  LTC4162_CHARGER_C_OVER_X_TERMINATION =
      (1 << 3), // Charge current fell below the full-charge threshold
  LTC4162_CHARGER_MAX_CHARGE_TIME =
      (1 << 2), // Maximum total safety charge time has been exceeded
  LTC4162_CHARGER_BAT_MISSING_FAULT =
      (1 << 1), // No battery detected while trying to initiate a charge cycle
  LTC4162_CHARGER_BAT_SHORT_FAULT =
      (1 << 0) // A shorted cell or short-circuit condition was detected on the
               // battery
} ltc4162_charger_state_t;

typedef enum {
  LTC4162_CHARGE_STATUS_OFF = 0, // Charger is off
  LTC4162_CHARGE_STATUS_CONSTANT_VOLTAGE =
      (1 << 0), // Constant-Voltage (CV) regulation loop is active
  LTC4162_CHARGE_STATUS_CONSTANT_CURRENT =
      (1 << 1), // Constant-Current (CC) regulation loop is active
  LTC4162_CHARGE_STATUS_IIN_LIMIT_ACTIVE =
      (1 << 2), // Input current limit loop is active
  LTC4162_CHARGE_STATUS_VIN_UVCL_ACTIVE =
      (1 << 3), // Input under-voltage current limit loop is active
  LTC4162_CHARGE_STATUS_THERMAL_REG_ACTIVE =
      (1 << 4), // Thermal regulation loop is active (reducing current due to
                // temperature)
  LTC4162_CHARGE_STATUS_ILIM_REG_ACTIVE =
      (1 << 5) // Output current limit loop is active
} ltc4162_charge_status_t;

typedef struct {
  float vbat;
  float vin;
  float ibat;
  float iin;
  float die_temp;
  float vout;
  int16_t system_status;
  bool charger_status;
  bool charger_state;
} ltc4162_charger_data_t;

/**
 * @brief Initialize the library.
 * @param[in] config pointer to `ltc4162_config_t` init structure.
 * @return `ESP_OK` if init succeeded, else error code.
 */
esp_err_t ltc4162_init(const ltc4162_config_t *config);

/**
 * @brief Deinitialize the library.
 * @return `ESP_OK` if init succeeded.
 */
esp_err_t ltc4162_deinit(void);

/**
 * @brief Enables LTC4162 hardware interrupts (alerts) on int_pin.
 * @param[in] limit_mask Bitmask of alerts to enable from
 * `ltc4162_alert_limit_t`. Set to `LTC4162_ALERT_LIMIT_NONE` to disable.
 * @param[in] state_mask Bitmask of alerts to enable from
 * `ltc4162_alert_state_t`. Set to `LTC4162_ALERT_STATE_NONE` to disable.
 * @param[in] status_mask Bitmask of alerts to enable from
 * `ltc4162_alert_status_t`. Set to `LTC4162_ALERT_STATUS_NONE` to disable.
 * @return `ESP_OK` on success.
 */
esp_err_t ltc4162_enable_interrupts(uint16_t limit_mask, uint16_t state_mask,
                                    uint16_t status_mask);

/**
 * @brief Reads the active alerts that triggered the interrupt.
 * @param[out] limit_alerts Pointer to store active alerts from
 * `ltc4162_alert_limit_t`.
 * @param[out] state_alerts Pointer to store active alerts from
 * `ltc4162_alert_state_t`.
 * @param[out] status_alerts Pointer to store active alerts from
 * `ltc4162_alert_status_t`.
 * @return `ESP_OK` on success.
 */
esp_err_t ltc4162_get_alerts(uint16_t *limit_alerts, uint16_t *state_alerts,
                             uint16_t *status_alerts);

/**
 * @param[out] data pointer to the structure to store data.
 * @return `ESP_OK` on success.
 */
esp_err_t read_charger_data(ltc4162_charger_data_t *data);

/**
 * @brief Suspends or resumes the battery charging process.
 * @warning This function should be used cautiously to avoid causing a system
 * deadlock.
 * @param[in] suspend `true` to suspend charging, `false` to resume.
 * @return `ESP_OK` on success.
 */
esp_err_t ltc4162_set_suspend(bool suspend);

// |--- OTHER ---|

esp_err_t ltc4162_debug_monitor(void);
esp_err_t ltc4162_read_register(uint8_t reg, uint8_t *data, uint8_t len);
esp_err_t ltc4162_write_register(uint8_t reg, uint8_t *data, uint8_t len);

const char *ltc4162_charger_state_to_str(ltc4162_charger_state_t state);
const char *ltc4162_charge_status_to_str(ltc4162_charge_status_t status);

#endif /* ltc4162_h */