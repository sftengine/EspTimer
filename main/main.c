#include <stdio.h>
#include <string.h>
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs.h"
#include "wifi_manager.h"
#include "ntp_sync.h"
#include "relay_control.h"
#include "timer_storage.h"
#include "https_server.h"

static const char *TAG = "main";

// Timer settings
static uint8_t on_hour = 8;
static uint8_t on_min = 0;
static uint8_t off_hour = 18;
static uint8_t off_min = 0;

/**
 * @brief Check if current time is between two times, handling midnight crossing
 * 
 * @param current_hour Current hour (0-23)
 * @param current_min Current minute (0-59)
 * @param start_hour Start hour (0-23)
 * @param start_min Start minute (0-59)
 * @param end_hour End hour (0-23)
 * @param end_min End minute (0-59)
 * @return true if current time is in range, false otherwise
 */
static bool is_time_in_range(uint8_t current_hour, uint8_t current_min,
                             uint8_t start_hour, uint8_t start_min,
                             uint8_t end_hour, uint8_t end_min)
{
    int current_minutes = current_hour * 60 + current_min;
    int start_minutes = start_hour * 60 + start_min;
    int end_minutes = end_hour * 60 + end_min;
    
    // Handle midnight crossing (e.g., 23:00 to 06:00)
    if (start_minutes > end_minutes) {
        // Range crosses midnight
        return (current_minutes >= start_minutes) || (current_minutes < end_minutes);
    } else {
        // Normal range within same day
        return (current_minutes >= start_minutes) && (current_minutes < end_minutes);
    }
}

/**
 * @brief Task to check timer and control relay
 */
static void timer_check_task(void *pvParameters)
{
    ESP_LOGI(TAG, "Timer check task started");
    
    while (1) {
        // Wait for 60 seconds
        vTaskDelay(60000 / portTICK_PERIOD_MS);
        
        // Reload timer settings in case they were updated
        esp_err_t ret = timer_load_settings(&on_hour, &on_min, &off_hour, &off_min);
        if (ret != ESP_OK && ret != ESP_ERR_NVS_NOT_FOUND) {
            ESP_LOGW(TAG, "Failed to load timer settings: %s", esp_err_to_name(ret));
            continue;
        }
        
        // Get current time
        struct tm timeinfo;
        get_current_time(&timeinfo);
        
        uint8_t current_hour = timeinfo.tm_hour;
        uint8_t current_min = timeinfo.tm_min;
        
        // Check if relay should be on
        bool should_be_on = is_time_in_range(current_hour, current_min,
                                              on_hour, on_min,
                                              off_hour, off_min);
        
        // Update relay state if needed
        bool current_state = relay_get_state();
        if (should_be_on != current_state) {
            relay_set_state(should_be_on);
            ESP_LOGI(TAG, "Timer triggered: Relay turned %s at %02d:%02d",
                     should_be_on ? "ON" : "OFF", current_hour, current_min);
        }
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "ESP8266 Web Timer Starting...");
    ESP_LOGI(TAG, "Free heap: %d bytes", esp_get_free_heap_size());

    // Initialize NVS
    ESP_LOGI(TAG, "Initializing NVS...");
    ESP_ERROR_CHECK(timer_storage_init());

    // Initialize WiFi
    ESP_LOGI(TAG, "Initializing WiFi...");
    ESP_ERROR_CHECK(wifi_init_sta());

    // Print IP address
    char ip_str[16];
    wifi_get_ip_address(ip_str, sizeof(ip_str));
    ESP_LOGI(TAG, "=====================================");
    ESP_LOGI(TAG, "WiFi Connected!");
    ESP_LOGI(TAG, "IP Address: %s", ip_str);
    ESP_LOGI(TAG, "Web Interface: https://%s", ip_str);
    ESP_LOGI(TAG, "=====================================");

    // Sync time with NTP
    ESP_LOGI(TAG, "Synchronizing time with NTP...");
    ESP_ERROR_CHECK(ntp_sync_time());

    // Initialize relay
    ESP_LOGI(TAG, "Initializing relay control...");
    relay_init();

    // Load timer settings
    ESP_LOGI(TAG, "Loading timer settings...");
    esp_err_t ret = timer_load_settings(&on_hour, &on_min, &off_hour, &off_min);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Loaded timer: ON=%02d:%02d OFF=%02d:%02d",
                 on_hour, on_min, off_hour, off_min);
    } else if (ret == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGI(TAG, "No saved timer settings, using defaults: ON=%02d:%02d OFF=%02d:%02d",
                 on_hour, on_min, off_hour, off_min);
    } else {
        ESP_LOGW(TAG, "Failed to load timer settings: %s", esp_err_to_name(ret));
    }

    // Start HTTPS server
    ESP_LOGI(TAG, "Starting HTTPS server...");
    //ESP_ERROR_CHECK(start_https_server());

    ESP_LOGI(TAG, "=====================================");
    ESP_LOGI(TAG, "System Ready!");
    ESP_LOGI(TAG, "Access web interface at: https://%s", ip_str);
    ESP_LOGI(TAG, "Note: Accept the self-signed certificate warning in your browser");
    ESP_LOGI(TAG, "=====================================");

    // Create timer check task
    TaskHandle_t timer_task_handle = NULL;
    BaseType_t task_created = xTaskCreate(timer_check_task, "timer_check", 2048, NULL, 5, &timer_task_handle);
    if (task_created != pdPASS) {
        ESP_LOGE(TAG, "Failed to create timer check task");
    } else {
        ESP_LOGI(TAG, "Timer check task created successfully");
    }

    ESP_LOGI(TAG, "Free heap after initialization: %d bytes", esp_get_free_heap_size());

    // Main loop - just monitor system status
    while (1) {
        vTaskDelay(10000 / portTICK_PERIOD_MS);
        
        // Log system status every 10 seconds
        if (!wifi_is_connected()) {
            ESP_LOGW(TAG, "WiFi connection lost!");
        }
        
        // Log current state periodically
        struct tm timeinfo;
        get_current_time(&timeinfo);
        ESP_LOGI(TAG, "Status - Time: %02d:%02d:%02d, Relay: %s, Heap: %d bytes",
                 timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec,
                 relay_get_state() ? "ON" : "OFF",
                 esp_get_free_heap_size());
    }
}
