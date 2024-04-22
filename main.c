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
#include "sounds/wave.h"
#include "drivers/distance.h"
#include "drivers/tlv320aic14kibt.h"
#include "sounds/wave.h"

/*******************************************************************************
 * External Global Variables
 ******************************************************************************/
volatile uint8_t range_sensor1 = 0;
volatile uint8_t status_sensor1;
volatile uint8_t range_sensor2 = 0;
volatile uint8_t status_sensor2;

// Function declarations
void task_read_serial();
void task_read_distance();
void task_lcd_status();
void task_blink_led();


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
    cyhal_gpio_init(P5_5, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, 1);
    cyhal_gpio_init(P7_2, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, 1);


    xTaskCreate(
        task_read_serial,
        "Serial Commands Task",
        configMINIMAL_STACK_SIZE,
        NULL,
        3,
        &console_task);

    xTaskCreate(
        task_read_distance,
        "Distance sensor task",
        configMINIMAL_STACK_SIZE,
        NULL,
        2,
        NULL);

    xTaskCreate(
        task_blink_led,
        "Led blink task",
        configMINIMAL_STACK_SIZE,
        NULL,
        1,
        NULL);

    xTaskCreate(
        task_lcd_status,
        "LCD Display task",
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

void task_blink_led() {
    TickType_t lastticktime = xTaskGetTickCount();
    for (;;) {
        xTaskDelayUntil( &lastticktime, 1000);
        cyhal_gpio_toggle(P7_2);
    }
}

void task_lcd_status() {

    backlight();
    char *sensor1valuestring = malloc(25*4);
    TickType_t lastticktime = xTaskGetTickCount();
    for (;;) {
        xTaskDelayUntil( &lastticktime, 500);
        cyhal_gpio_toggle(P5_5);
        sprintf(sensor1valuestring, "Range 1:%3d%*sRange 2:%3d", range_sensor1, 9, "", range_sensor2);
        writeString(sensor1valuestring);        
    }
    // cyhal_system_delay_ms(5000);
}

void task_read_distance() {

    TickType_t lastticktime = xTaskGetTickCount();
    for (;;) {
        xTaskDelayUntil( &lastticktime, 100);
        setAddress(0x29); // This is the sensor WITH GPIO0 connected
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
    uint32_t ulNotifiedValue;
    for (;;) {
        xTaskNotifyWait(0x00, 0, &ulNotifiedValue, portMAX_DELAY);
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
        else if (strcmp(args[0], "i2s") == 0) {
            /* Start the I2S TX */
            cyhal_i2s_start_tx(&i2s);

            /* If not transmitting, initiate a transfer */
            cyhal_i2s_write_async(&i2s, wave_data, WAVE_SIZE);
        }
        else if (strcmp(args[0], "i2s") == 0) {
            /* Start the I2S TX */
            cyhal_i2s_start_tx(&i2s);

            /* If not transmitting, initiate a transfer */
            cyhal_i2s_write_async(&i2s, wave_data, WAVE_SIZE);
        }
        cInputIndex = 0;
        free(args);
    }
}