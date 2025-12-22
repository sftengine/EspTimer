#ifndef HTTPS_SERVER_H
#define HTTPS_SERVER_H

#include "esp_err.h"

/**
 * @brief Start HTTPS server
 * 
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t start_https_server(void);

/**
 * @brief Stop HTTPS server
 */
void stop_https_server(void);

#endif // HTTPS_SERVER_H
