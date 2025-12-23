#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "lwip/apps/sntp.h"

#include "ntp_sync.h"

static const char *TAG = "ntp_sync";
static bool time_synced = false;

void time_sync_notification_cb(struct timeval *tv)
{
    ESP_LOGI(TAG, "Time synchronized");
    time_synced = true;
}

esp_err_t ntp_sync_time(void)
{
    ESP_LOGI(TAG, "Initializing SNTP");
    
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, CONFIG_NTP_SERVER_1);
    sntp_setservername(1, CONFIG_NTP_SERVER_2);
    sntp_setservername(2, CONFIG_NTP_SERVER_3);
    
    sntp_set_time_sync_notification_cb(time_sync_notification_cb);
    sntp_init();

    // Wait for time to be set
    int retry = 0;
    const int retry_count = 15;
    
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) {
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }

    if (retry >= retry_count) {
        ESP_LOGE(TAG, "Failed to sync time with NTP servers");
        return ESP_FAIL;
    }

    // Set timezone
    // POSIX TZ format: positive CONFIG_TIMEZONE_OFFSET means hours AHEAD of UTC
    // But POSIX format uses negative offset for ahead (e.g., "UTC-1" for CET which is UTC+1)
    // So we negate the user-friendly CONFIG_TIMEZONE_OFFSET value
    char tz_str[32];
    snprintf(tz_str, sizeof(tz_str), "UTC%+d", -CONFIG_TIMEZONE_OFFSET);
    setenv("TZ", tz_str, 1);
    tzset();

    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    
    ESP_LOGI(TAG, "Current time: %04d-%02d-%02d %02d:%02d:%02d",
             timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
             timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

    return ESP_OK;
}

void get_current_time_str(char *buffer, size_t buffer_size)
{
    if (buffer == NULL || buffer_size == 0) {
        return;
    }

    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    
    // Format: "2025-12-21 14:35:22"
    strftime(buffer, buffer_size, "%Y-%m-%d %H:%M:%S", &timeinfo);
}

void get_current_time(struct tm *timeinfo)
{
    if (timeinfo == NULL) {
        return;
    }

    time_t now;
    time(&now);
    localtime_r(&now, timeinfo);
}

bool is_time_synced(void)
{
    return time_synced;
}
