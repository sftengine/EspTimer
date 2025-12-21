#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "esp_err.h"

/**
 * @brief Initialize and connect to WiFi
 * 
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t wifi_init_sta(void);

/**
 * @brief Check if WiFi is connected
 * 
 * @return true if connected, false otherwise
 */
bool wifi_is_connected(void);

/**
 * @brief Get the IP address as string
 * 
 * @param ip_str Buffer to store IP address string
 * @param max_len Maximum length of buffer
 */
void wifi_get_ip_address(char *ip_str, size_t max_len);

#endif // WIFI_MANAGER_H
