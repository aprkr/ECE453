#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

/* RTOS header files */
#include <FreeRTOS.h>
#include <FreeRTOSConfig.h>
#include <task.h>
#include <semphr.h>
#include <event_groups.h>

/* Drivers */
#include "drivers/console.h"
#include "drivers/i2c.h"
#include "drivers/lcd.h"
#include "drivers/adc.h"

/*******************************************************************************
 * External Global Variables
 ******************************************************************************/

void task_read_serial(); //

int main(void)
{
    cy_rslt_t rslt;

    /* Initialize the device and board peripherals */
    rslt = cybsp_init();
    CY_ASSERT(CY_RSLT_SUCCESS == rslt);

    console_init();

    printf("\x1b[2J\x1b[;H");
    printf("Theremin Serial Interface\n\r");

    __enable_irq();

    i2c_init();
    lcdbegin(20,4);
    adc_init();

    xTaskCreate(
        task_read_serial,
        "Serial Commands Task",
        configMINIMAL_STACK_SIZE,
        NULL,
        1,
        NULL);

    // Start the scheduler
    vTaskStartScheduler();

    for (;;)
    {
    }
}

void task_read_serial() {
    for (;;) {
        if (ALERT_CONSOLE_RX) {
            char **args = (char **)malloc(128 * sizeof(char*));
            int argc = 0;
            char *input = strtok(pcInputString, " ");
            while(input != NULL) {
                args[argc] = input;
                argc++;
                input = strtok(NULL, " ");
            }
            args = (char **)realloc(args, argc * sizeof(char*));

            printf("Command received %s\n\r",args[0]);

            if (strcmp(args[0], "lcd") == 0) {
                writeString(args[1]);
            }
            else if (strcmp(args[0], "adc") == 0) {
                printf("pot 1 %li\n\r", read_adc(adc_chan_0_obj));
                printf("pot 2 %li\n\r", read_adc(adc_chan_1_obj));
            }
            ALERT_CONSOLE_RX = false;
            cInputIndex = 0;
            free(args);
        }
    }
}