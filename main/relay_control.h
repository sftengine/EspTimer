#ifndef RELAY_CONTROL_H
#define RELAY_CONTROL_H

#include <stdbool.h>

/**
 * @brief Initialize relay GPIO
 */
void relay_init(void);

/**
 * @brief Set relay state
 * 
 * @param state true to turn relay ON, false to turn OFF
 */
void relay_set_state(bool state);

/**
 * @brief Get current relay state
 * 
 * @return true if relay is ON, false if OFF
 */
bool relay_get_state(void);

/**
 * @brief Toggle relay state
 */
void relay_toggle(void);

#endif // RELAY_CONTROL_H
