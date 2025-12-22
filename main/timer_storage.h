#ifndef TIMER_STORAGE_H
#define TIMER_STORAGE_H

#include "esp_err.h"
#include <stdint.h>

/**
 * @brief Initialize timer storage (NVS)
 * 
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t timer_storage_init(void);

/**
 * @brief Save timer settings to NVS
 * 
 * @param on_hour Hour to turn relay ON (0-23)
 * @param on_min Minute to turn relay ON (0-59)
 * @param off_hour Hour to turn relay OFF (0-23)
 * @param off_min Minute to turn relay OFF (0-59)
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t timer_save_settings(uint8_t on_hour, uint8_t on_min, 
                               uint8_t off_hour, uint8_t off_min);

/**
 * @brief Load timer settings from NVS
 * 
 * @param on_hour Pointer to store ON hour
 * @param on_min Pointer to store ON minute
 * @param off_hour Pointer to store OFF hour
 * @param off_min Pointer to store OFF minute
 * @return ESP_OK on success, ESP_ERR_NVS_NOT_FOUND if not set, other error code otherwise
 */
esp_err_t timer_load_settings(uint8_t *on_hour, uint8_t *on_min,
                               uint8_t *off_hour, uint8_t *off_min);

#endif // TIMER_STORAGE_H
