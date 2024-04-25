#include "gpio.h"

void gpio_init() {
    cyhal_gpio_init(BLINK_TASK_PIN_EXTRA, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, 1);
    cyhal_gpio_init(BLINK_TASK_PIN, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, 1);
    cyhal_gpio_init(SWITCH_PIN, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_NONE, false);
    cyhal_gpio_init(BUTTON_PIN, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_NONE, false);
}