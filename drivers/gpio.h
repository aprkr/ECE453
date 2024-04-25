#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"

#define BLINK_TASK_PIN P7_2
#define BLINK_TASK_PIN_EXTRA P5_5
#define BUTTON_PIN P7_7
#define SWITCH_PIN P6_3

void gpio_init();