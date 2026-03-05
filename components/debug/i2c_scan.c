#include "esp_log.h"
#include "mcu_i2c_config.h"

void i2c_scan() {
    ESP_LOGI("I2C_SCAN", "Starting I2C scan...");

    for (uint8_t address = 1; address < 127; address++) {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_WRITE, true);
        i2c_master_stop(cmd);

        esp_err_t ret = i2c_master_cmd_begin(CONFIG_I2C_MASTER_PORT_NUM, cmd, pdMS_TO_TICKS(CONFIG_I2C_MASTER_TIMEOUT_MS));
        if (ret == ESP_OK) {
            ESP_LOGI("I2C_SCAN", "Device found at address: 0x%02X", address);
        } else if (ret != ESP_ERR_TIMEOUT) {
            continue;
        }

        i2c_cmd_link_delete(cmd);
    }

    ESP_LOGI("I2C_SCAN", "I2C scan completed.");
}
