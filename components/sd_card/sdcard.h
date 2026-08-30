// Copyright 2026 PWrInSpace, Mateusz Kluczka

#pragma once

#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/unistd.h>

#include "driver/sdmmc_host.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"

#define SDCARD_MOUNT_POINT "/sdcard"
#define SD_CREATE_FILE_PREFIX(usr_path) SDCARD_MOUNT_POINT "/" usr_path
// 8 is a placeholder for _number_.txt
#define PATH_FLIE_SIZE(usr_path) sizeof(SD_CREATE_FILE_PREFIX(usr_path)) + 8

#define SD_CLK_PIN 14
#define SD_CMD_PIN 21
#define SD_D0_PIN 13
#define SD_D1_PIN 12
#define SD_D2_PIN 48
#define SD_D3_PIN 47
#define SD_CD_PIN                                                              \
  GPIO_NUM_NC // GPIO_NUM_NC oznacza brak pinu Card Detect (zawsze włożona)

typedef struct {
  sdmmc_card_t *card;
  const char *mount_point;
  int card_detect_pin;
  bool mounted;
} sd_card_t;

typedef struct {
  const char *mount_point;
  int cd_pin;
} sd_card_config_t;

/*!
 * \brief Initialize sd card
 *
 * \param sd_card pointer to sd_card_t struct
 * \param host spi_host device
 * \param cs chip select pin
 * \param m_point mounting point
 * \returns True if write operation successful, false otherwise
 */
bool SD_init(sd_card_t *sd_card, sd_card_config_t *cfg);

/*!
 * \brief Write string to sd card
 *
 * \param sd_card pointer to sd_card_t struct
 * \param path path to file
 * \param data data
 * \param length data length
 * \returns True if write operation successful, false otherwise
 */
bool SD_write(sd_card_t *sd_card, const char *path, const char *data,
              size_t length);

/*!
 * \brief Check if file exists
 *
 * \param path path to file
 * \returns True if file exists, false otherwise
 */
bool SD_file_exists(const char *path);

/*!
 * \brief Mount sd card in case of unmounted
 *
 * \param sd_card sd card struct
 * \returns True if mount is successful, false otherwise
 */
bool SD_mount(sd_card_t *sd_card);

/*!
 * \brief Remount sd card
 *
 * \param sd_card pointer to sd_card_t struct
 * \returns True if remount is successful, false otherwise
 */
bool SD_remount(sd_card_t *sd_card);

/*!
 * \brief Unmount sd card
 *
 * \param sd_card pointer to sd_card_t struct
 * \returns True if unmount is successful, false otherwise
 */
bool SD_unmount(sd_card_t *sd_card);

/*!
 * \brief Checks SD Card status
 *
 * \param sd_card sd struct
 * \returns true sd is fine, false otherwise
 */
bool SD_is_ok(sd_card_t *sd_card);

/*!
 * \brief Check if SD card is detected through CD pin
 * \param sd_card sd struct
 */
bool SD_card_detect(sd_card_t *sd_card);

/*!
 * \brief Create a unique path to file object
 *
 * \param file_path name of file
 * \param size size of file path buffer
 * \returns true if unique path created, false otherwise
 */
bool create_path_to_file(char *file_path, size_t size);