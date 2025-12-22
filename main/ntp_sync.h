#ifndef NTP_SYNC_H
#define NTP_SYNC_H

#include "esp_err.h"
#include <time.h>

/**
 * @brief Initialize and synchronize time with NTP servers
 * 
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t ntp_sync_time(void);

/**
 * @brief Get current time as string
 * 
 * @param buffer Buffer to store time string
 * @param buffer_size Size of buffer
 */
void get_current_time_str(char *buffer, size_t buffer_size);

/**
 * @brief Get current time structure
 * 
 * @param timeinfo Pointer to tm structure to fill
 */
void get_current_time(struct tm *timeinfo);

/**
 * @brief Check if time is synchronized
 * 
 * @return true if time is set, false otherwise
 */
bool is_time_synced(void);

#endif // NTP_SYNC_H
