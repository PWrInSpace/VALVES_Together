// Copyright 2026 PWrInSpace, Mateusz Kluczka

#include "sdcard.h"

#include "sd_test_io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "driver/gpio.h"
#include "esp_log.h"

#define TAG "SDCARD"

bool SD_init(sd_card_t *sd_card, sd_card_config_t *cfg) {
  if (sd_card == NULL || cfg == NULL)
    return false;

  sd_card->card_detect_pin = cfg->cd_pin;
  sd_card->mount_point =
      cfg->mount_point ? cfg->mount_point : SDCARD_MOUNT_POINT;
  sd_card->mounted = false;

  return SD_mount(sd_card);
}

bool SD_file_exists(const char *file_name) {
  struct stat st;
  return (stat(file_name, &st) == 0);
}

bool SD_mount(sd_card_t *sd_card) {
  esp_err_t res;

  sdmmc_host_t host = SDMMC_HOST_DEFAULT();
  sdmmc_slot_config_t slot_config = {.clk = SD_CLK_PIN,
                                     .cmd = SD_CMD_PIN,
                                     .d0 = SD_D0_PIN,
                                     .d1 = SD_D1_PIN,
                                     .d2 = SD_D2_PIN,
                                     .d3 = SD_D3_PIN,
                                     .cd = SDMMC_SLOT_NO_CD,
                                     .wp = SDMMC_SLOT_NO_WP,
                                     .width = 4,
                                     .flags = SDMMC_SLOT_FLAG_INTERNAL_PULLUP};

  esp_vfs_fat_sdmmc_mount_config_t mount_config = {
      .format_if_mount_failed = false,
      .max_files = 5,
      .allocation_unit_size = 16 * 1024};

  res = esp_vfs_fat_sdmmc_mount(sd_card->mount_point, &host, &slot_config,
                                &mount_config, &sd_card->card);

  if (res != ESP_OK) {
    ESP_LOGE("SD_INIT", "esp_vfs_fat_sdspi_mount failed: %s (0x%x)",
             esp_err_to_name(res), res);

    if (res == ESP_FAIL) {
      ESP_LOGE(TAG, "Failed to mount filesystem. "
                    "If you want the card to be formatted, set the"
                    "CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
    } else {
      ESP_LOGE(TAG,
               "Failed to initialize the card (%s). "
               "Make sure SD card lines have pull-up resistors in place.",
               esp_err_to_name(res));
      // check_sd_card_pins(&pin_config, pin_count);
    }
    return false;
  }

  ESP_LOGI(TAG, "SD card mounted");
  sd_card->mounted = true;
  return true;
}

bool SD_unmount(sd_card_t *sd_card) {
  if (!sd_card->mounted)
    return true;

  esp_err_t res =
      esp_vfs_fat_sdcard_unmount(sd_card->mount_point, sd_card->card);
  if (res != ESP_OK) {
    ESP_LOGE(TAG, "UNMOUNT ERROR (0x%x)\n", res);
    return false;
  }

  sd_card->mounted = false;
  return true;
}

bool SD_remount(sd_card_t *sd_card) {
  if (!SD_unmount(sd_card))
    return false;
  return SD_mount(sd_card);
}

bool SD_write(sd_card_t *sd_card, const char *path, const char *data,
              size_t length) {
  esp_err_t res;

  if (!sd_card->mounted) {
    if (!SD_mount(sd_card))
      return false;
  }

  res = sdmmc_get_status(sd_card->card);
  if (res != ESP_OK) {
    ESP_LOGE(TAG, "CARD ERROR (0x%x), REMOUNTING...", res);
    SD_remount(sd_card);
  }

  FILE *file = fopen(path, "a");
  if (file == NULL) {
    ESP_LOGE(TAG, "Base path %s", sd_card->mount_point);
    ESP_LOGE(TAG, "FILE OPEN ERROR %s", path);
    return false;
  }

  int written_bytes = fprintf(file, data, sd_card->card->cid.name);
  fclose(file);

  if (written_bytes < 1) {
    ESP_LOGE(TAG, "UNABLE TO WRITE DATA TO SD CARD");
    return false;
  }

  return true;
}

bool SD_is_ok(sd_card_t *sd_card) {
  esp_err_t res = sdmmc_get_status(sd_card->card);
  if (res != ESP_OK) {
    ESP_LOGE(TAG, "SD error status %s", esp_err_to_name(res));
    return false;
  }

  return true;
}

bool SD_card_detect(sd_card_t *sd_card) {
  if (sd_card->card_detect_pin == GPIO_NUM_NC)
    return true; // Brak pinu CD = karta traktowana jako zawsze obecna
  return (gpio_get_level(sd_card->card_detect_pin) == 0);
}

bool create_path_to_file(char *file_path, size_t size) {
  char *path = (char *)calloc(size, sizeof(char));
  if (path == NULL)
    return false;

  int ret = 0;
  for (int i = 0; i < 1000; ++i) {
    ret = snprintf(path, size, "%s%d.txt", file_path, i);
    if (ret == size) {
      free(path);
      return false;
    }

    if (SD_file_exists(path) == false) {
      memcpy(file_path, path, size);
      free(path);
      return true;
    }
  }

  free(path);
  return false;
}