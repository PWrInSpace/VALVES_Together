#include "thermocouple_config.h"

#include "driver/spi_master.h"
#include "esp_log.h"

static const char *TAG = "THERMOCOUPLE_CONFIG";

static const gpio_num_t tc_cs_pins[THERMOCOUPLE_COUNT] = {
    TC0_CS_GPIO, TC1_CS_GPIO, TC2_CS_GPIO};

max31856_cfg thermocouple_devices[THERMOCOUPLE_COUNT];

static bool tc_spi_bus_initialized = false;

esp_err_t thermocouple_config_init(void) {
  if (!tc_spi_bus_initialized) {
    spi_bus_config_t buscfg = {
        .miso_io_num = TC_MISO_GPIO,
        .mosi_io_num = TC_MOSI_GPIO,
        .sclk_io_num = TC_SCK_GPIO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 32,
    };

    esp_err_t ret = spi_bus_initialize(TC_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
      ESP_LOGE(TAG, "Thermocouple SPI bus init failed: %s",
               esp_err_to_name(ret));
      return ret;
    }
    tc_spi_bus_initialized = true;
    ESP_LOGI(TAG, "Thermocouple SPI bus initialized (SCK=%d MISO=%d MOSI=%d)",
             TC_SCK_GPIO, TC_MISO_GPIO, TC_MOSI_GPIO);
  }

  for (int i = 0; i < THERMOCOUPLE_COUNT; i++) {
    if (!max31856_init(&thermocouple_devices[i], tc_cs_pins[i], TC_SPI_HOST)) {
      ESP_LOGE(TAG, "MAX31856 init failed for TC%d (CS GPIO%d)", i,
               tc_cs_pins[i]);
      return ESP_FAIL;
    }
    thermocouple_set_type(&thermocouple_devices[i], THERMOCOUPLE_TYPE);
    ESP_LOGI(TAG, "MAX31856 TC%d initialized (CS GPIO%d)", i, tc_cs_pins[i]);
  }

  return ESP_OK;
}
