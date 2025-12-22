#include "driver/gpio.h"
#include "esp_log.h"
#include "relay_control.h"

static const char *TAG = "relay_control";
static bool relay_state = false;

void relay_init(void)
{
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << CONFIG_RELAY_GPIO_PIN),
        .pull_down_en = 0,
        .pull_up_en = 0,
    };
    
    gpio_config(&io_conf);
    
    // Initialize relay to OFF state
    relay_set_state(false);
    
    ESP_LOGI(TAG, "Relay initialized on GPIO %d", CONFIG_RELAY_GPIO_PIN);
}

void relay_set_state(bool state)
{
    relay_state = state;
    
#ifdef CONFIG_RELAY_ACTIVE_HIGH
    gpio_set_level(CONFIG_RELAY_GPIO_PIN, state ? 1 : 0);
#else
    gpio_set_level(CONFIG_RELAY_GPIO_PIN, state ? 0 : 1);
#endif
    
    ESP_LOGI(TAG, "Relay set to %s", state ? "ON" : "OFF");
}

bool relay_get_state(void)
{
    return relay_state;
}

void relay_toggle(void)
{
    relay_set_state(!relay_state);
}
