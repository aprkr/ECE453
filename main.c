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
#include "drivers/gpio.h"

#include "sounds/A.h"
#include "sounds/B.h"
#include "sounds/C.h"
#include "sounds/D.h"
#include "sounds/E.h"
#include "sounds/F.h"
#include "sounds/G.h"

/*******************************************************************************
 * External Global Variables
 ******************************************************************************/
volatile uint8_t range_sensor1 = 0;
volatile uint8_t status_sensor1;
volatile uint8_t range_sensor2 = 0;
volatile uint8_t status_sensor2;
volatile uint32_t adc0_value;
volatile uint32_t adc1_value;
volatile uint8_t curLcdMode = 0;
volatile uint8_t buttonPressed = 0;
volatile uint8_t scale;

// Function declarations
void task_read_serial();
void task_read_distance();
void task_lcd_status();
void task_blink_led();
void task_read_adc();
void task_read_switch();


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
    distancebegin();
    i2s_init();
    gpio_init();


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

    xTaskCreate(
        task_read_adc,
        "ADC read task",
        configMINIMAL_STACK_SIZE,
        NULL,
        1,
        NULL);

    xTaskCreate(
        task_read_switch,
        "Switch and button read task",
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

void task_read_switch() {
    TickType_t lastticktime = xTaskGetTickCount();
    for (;;) {
        cyhal_gpio_toggle(BLINK_TASK_PIN_EXTRA);
        xTaskDelayUntil( &lastticktime, 200);
        if (cyhal_gpio_read(BUTTON_PIN) == false && buttonPressed == 0) {
            buttonPressed ^= 1;
            curLcdMode ^= 1;
            if (curLcdMode == 0) {
                writeString("Range 1:\nRange 2:\nADC value 1:\nADC value 2:");
            } else if (curLcdMode == 1) {
                writeString("User mode");
            }
        } else if (cyhal_gpio_read(BUTTON_PIN) == true && buttonPressed == 1) {
            buttonPressed ^= 1;
        }
        if (cyhal_gpio_read(SWITCH_PIN) == true) { // having true = off
            noBacklight();
            vTaskSuspendAll();
            while(cyhal_gpio_read(SWITCH_PIN) == true) {
                cyhal_syspm_sleep();
            }
            __NVIC_SystemReset();
        }
    }
}

void task_read_adc() {
    TickType_t lastticktime = xTaskGetTickCount();
    for (;;) {
        xTaskDelayUntil( &lastticktime, 200);
        adc0_value = read_adc(adc_chan_0_obj);
        adc1_value = read_adc(adc_chan_1_obj);
    }
}

void task_blink_led() {
    TickType_t lastticktime = xTaskGetTickCount();
    for (;;) {
        xTaskDelayUntil( &lastticktime, 1000);
        cyhal_gpio_toggle(BLINK_TASK_PIN);
    }
}

void task_lcd_status() {

    backlight();
    char *lcdstring = malloc(25*4);
    if (curLcdMode == 0) {
        sprintf(lcdstring, "Range 1:\nRange 2:\nADC value 1:\nADC value 2:");
    } else {
        sprintf(lcdstring, "User Mode Test");
    }
    writeString(lcdstring);
    TickType_t lastticktime = xTaskGetTickCount();
    for (;;) {
        xTaskDelayUntil( &lastticktime, 500);
        if (curLcdMode == 0) {
            sprintf(lcdstring,"%3d",range_sensor1);
            writeStringWithoutClear(0, 8, lcdstring, 3);
            sprintf(lcdstring,"%3d",range_sensor2);
            writeStringWithoutClear(1, 8, lcdstring, 3);
            sprintf(lcdstring,"%7lu",adc0_value);
            writeStringWithoutClear(2, 12, lcdstring, 7);
            sprintf(lcdstring,"%7lu",adc1_value);
            writeStringWithoutClear(3, 12, lcdstring, 7);
        } else {
            sprintf(lcdstring, "User Mode Task");
            writeStringWithoutClear(1, 0, lcdstring, 4);
        }
    }
}

void task_read_distance() {

    TickType_t lastticktime = xTaskGetTickCount();
    for (;;) {
        xTaskDelayUntil( &lastticktime, 100);
        // setAddress(0x29); // This is the sensor WITH GPIO0 connected
        // range_sensor1 = readRange();
        // status_sensor1 = readRangeStatus();
        // if (status_sensor1 != VL6180X_ERROR_NONE) {
        //     range_sensor1 = -1;
        // }
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
            TickType_t lastticktime = xTaskGetTickCount();
            for(int i = 0; i < 48; i++){
                switch(scale){
                    case 0:
                        cyhal_i2s_start_tx(&i2s);
                        cyhal_i2s_write_async(&i2s, C, C_size/16);
                        printf("HERE C\n");
                        while(cyhal_i2s_is_tx_busy(&i2s)){
                            xTaskDelayUntil( &lastticktime, 10);
                        }
                        cyhal_i2s_stop_tx(&i2s);
                        scale++;
                        break;
                    case 1:
                        cyhal_i2s_start_tx(&i2s);
                        cyhal_i2s_write_async(&i2s, D, D_size/16);
                        printf("HERE D\n");
                        while(cyhal_i2s_is_tx_busy(&i2s)){
                            xTaskDelayUntil( &lastticktime, 10);
                        }
                        cyhal_i2s_stop_tx(&i2s);
                        scale++;
                        break;
                    case 2:
                        cyhal_i2s_start_tx(&i2s);
                        cyhal_i2s_write_async(&i2s, E, E_size/16);
                        printf("HERE E\n");
                        while(cyhal_i2s_is_tx_busy(&i2s)){
                            xTaskDelayUntil( &lastticktime, 10);
                        }
                        cyhal_i2s_stop_tx(&i2s);
                        scale++;
                        break;
                    case 3:
                        cyhal_i2s_start_tx(&i2s);
                        cyhal_i2s_write_async(&i2s, F, F_size/16);
                        printf("HERE F\n");
                        while(cyhal_i2s_is_tx_busy(&i2s)){
                            xTaskDelayUntil( &lastticktime, 10);
                        }
                        cyhal_i2s_stop_tx(&i2s);
                        scale++;
                        break;
                    case 4:
                        cyhal_i2s_start_tx(&i2s);
                        cyhal_i2s_write_async(&i2s, G, G_size/16);
                        printf("HERE G\n");
                        cyhal_i2s_start_tx(&i2s);
                        while(cyhal_i2s_is_tx_busy(&i2s)){
                            xTaskDelayUntil( &lastticktime, 10);
                        }
                        cyhal_i2s_stop_tx(&i2s);
                        scale++;
                        break;
                    case 5:
                        cyhal_i2s_start_tx(&i2s);
                        cyhal_i2s_write_async(&i2s, A, A_size/16);
                        printf("HERE A\n");
                        cyhal_i2s_start_tx(&i2s);
                        while(cyhal_i2s_is_tx_busy(&i2s)){
                            xTaskDelayUntil( &lastticktime, 10);
                        }
                        cyhal_i2s_stop_tx(&i2s);
                        scale++;
                        break;
                    case 6:
                        cyhal_i2s_start_tx(&i2s);
                        cyhal_i2s_write_async(&i2s, B, B_size/16);
                        printf("HERE B\n");
                        cyhal_i2s_start_tx(&i2s);
                        while(cyhal_i2s_is_tx_busy(&i2s)){
                            xTaskDelayUntil( &lastticktime, 10);
                        }
                        cyhal_i2s_stop_tx(&i2s);
                        scale++;
                        break;
                    case 7:
                        cyhal_i2s_start_tx(&i2s);
                        cyhal_i2s_write_async(&i2s, C, C_size/16);
                        printf("HERE C\n");
                        cyhal_i2s_start_tx(&i2s);
                        while(cyhal_i2s_is_tx_busy(&i2s)){
                            xTaskDelayUntil( &lastticktime, 10);
                        }
                        cyhal_i2s_stop_tx(&i2s);
                        scale++;
                        break;
                    default:
                        scale = 0;
                        break;
                    }
            }
        }
        cInputIndex = 0;
        free(args);
    }
}