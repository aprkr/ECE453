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
#include "drivers/i2s.h"
#include "drivers/distance.h"
#include "drivers/tlv320aic14kibt.h"
#include "sounds/wave.h"

/*******************************************************************************
 * External Global Variables
 ******************************************************************************/
volatile uint8_t range_sensor1 = -1;
volatile uint8_t status_sensor1;
volatile uint8_t range_sensor2 = -1;
volatile uint8_t status_sensor2;

// Function declarations
void task_read_serial();
void task_read_distance();

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
    //lcdbegin(20,4);
    //adc_init();
    //distancebegin();
    i2s_init();

    xTaskCreate(
        task_read_serial,
        "Serial Commands Task",
        configMINIMAL_STACK_SIZE,
        NULL,
        1,
        NULL);

    // xTaskCreate(
    //     task_read_distance,
    //     "Distance sensor task",
    //     configMINIMAL_STACK_SIZE,
    //     NULL,
    //     1,
    //     NULL);

    // Start the scheduler
    vTaskStartScheduler();

    for (;;)
    {
    }
}

void task_read_distance() {
    for (;;) {
        setAddress(0x29);
        range_sensor1 = readRange();
        status_sensor1 = readRangeStatus();
        if (status_sensor1 != VL6180X_ERROR_NONE) {
            range_sensor1 = -1;
        }
        setAddress(0x30);
        range_sensor2 = readRange();
        status_sensor2 = readRangeStatus();
        if (status_sensor2 != VL6180X_ERROR_NONE) {
            range_sensor2 = -1;
        }
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
            } else if (strcmp(args[0], "dist") == 0) {
                printf("\n\rFirst sensor ");
                printf("Range: %d\n\r", range_sensor1);
                printf("Second sensor ");
                printf("Range: %d\n\r", range_sensor2);
            } else if (strcmp(args[0], "i2s") == 0) {
                tlv320aic14kibt_init();
                /* Start the I2S TX */
                cyhal_i2s_start_tx(&i2s);

                /* If not transmitting, initiate a transfer */
                cyhal_i2s_write_async(&i2s, __wave_wav, __wave_wav_size);
            }
            ALERT_CONSOLE_RX = false;
            cInputIndex = 0;
            free(args);
        }
    }
}