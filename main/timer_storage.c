#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
#include "timer_storage.h"

static const char *TAG = "timer_storage";
static const char *NVS_NAMESPACE = "timer";

esp_err_t timer_storage_init(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "NVS partition was truncated and needs to be erased");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "NVS initialized successfully");
    } else {
        ESP_LOGE(TAG, "Failed to initialize NVS: %s", esp_err_to_name(ret));
    }
    
    return ret;
}

esp_err_t timer_save_settings(uint8_t on_hour, uint8_t on_min, 
                               uint8_t off_hour, uint8_t off_min)
{
    nvs_handle_t my_handle;
    esp_err_t ret;

    // Validate inputs
    if (on_hour > 23 || on_min > 59 || off_hour > 23 || off_min > 59) {
        ESP_LOGE(TAG, "Invalid timer settings");
        return ESP_ERR_INVALID_ARG;
    }

    ret = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &my_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error opening NVS handle: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = nvs_set_u8(my_handle, "on_hour", on_hour);
    if (ret != ESP_OK) goto error;
    
    ret = nvs_set_u8(my_handle, "on_min", on_min);
    if (ret != ESP_OK) goto error;
    
    ret = nvs_set_u8(my_handle, "off_hour", off_hour);
    if (ret != ESP_OK) goto error;
    
    ret = nvs_set_u8(my_handle, "off_min", off_min);
    if (ret != ESP_OK) goto error;

    ret = nvs_commit(my_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error committing to NVS: %s", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Timer settings saved: ON=%02d:%02d OFF=%02d:%02d",
                 on_hour, on_min, off_hour, off_min);
    }

error:
    nvs_close(my_handle);
    return ret;
}

esp_err_t timer_load_settings(uint8_t *on_hour, uint8_t *on_min,
                               uint8_t *off_hour, uint8_t *off_min)
{
    nvs_handle_t my_handle;
    esp_err_t ret;

    if (on_hour == NULL || on_min == NULL || off_hour == NULL || off_min == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = nvs_open(NVS_NAMESPACE, NVS_READONLY, &my_handle);
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "Error opening NVS handle: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = nvs_get_u8(my_handle, "on_hour", on_hour);
    if (ret != ESP_OK) goto error;
    
    ret = nvs_get_u8(my_handle, "on_min", on_min);
    if (ret != ESP_OK) goto error;
    
    ret = nvs_get_u8(my_handle, "off_hour", off_hour);
    if (ret != ESP_OK) goto error;
    
    ret = nvs_get_u8(my_handle, "off_min", off_min);
    if (ret != ESP_OK) goto error;

    ESP_LOGI(TAG, "Timer settings loaded: ON=%02d:%02d OFF=%02d:%02d",
             *on_hour, *on_min, *off_hour, *off_min);

error:
    nvs_close(my_handle);
    
    if (ret == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGI(TAG, "No timer settings found in NVS, using defaults");
        // Set default values
        *on_hour = 8;
        *on_min = 0;
        *off_hour = 18;
        *off_min = 0;
        return ESP_OK;  // Return success when using defaults
    }
    
    return ret;
}
