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
#include "drivers/gpio.h"

#include "sounds/162Hz.h"
#include "sounds/172Hz.h"
#include "sounds/182Hz.h"
#include "sounds/192Hz.h"
#include "sounds/202Hz.h"
#include "sounds/212Hz.h"
#include "sounds/222Hz.h"
#include "sounds/232Hz.h"
#include "sounds/242Hz.h"
#include "sounds/252Hz.h"
#include "sounds/262Hz.h"
#include "sounds/272Hz.h"
#include "sounds/282Hz.h"
#include "sounds/292Hz.h"
#include "sounds/302Hz.h"
#include "sounds/312Hz.h"
#include "sounds/322Hz.h"
#include "sounds/332Hz.h"
#include "sounds/342Hz.h"
#include "sounds/352Hz.h"
#include "sounds/362Hz.h"
#include "sounds/372Hz.h"
#include "sounds/382Hz.h"
#include "sounds/392Hz.h"
#include "sounds/402Hz.h"
#include "sounds/412Hz.h"
#include "sounds/422Hz.h"
#include "sounds/432Hz.h"
#include "sounds/442Hz.h"
#include "sounds/452Hz.h"
#include "sounds/462Hz.h"

/*******************************************************************************
 * External Global Variables
 ******************************************************************************/
volatile uint8_t range_sensor1 = 0;
volatile uint8_t status_sensor1;
volatile uint8_t range_sensor2 = 0; // CHANGE THIS SO THAT WE STILL 262 HZ ON STARTUP
volatile uint8_t status_sensor2;
volatile uint32_t adc0_value;
volatile uint32_t adc1_value;
volatile uint8_t curLcdMode = 0;
volatile uint8_t buttonPressed = 0;
volatile uint8_t scale;
volatile uint16_t curWave[1600];
volatile int curWaveSize = 1600;

// Function declarations
void task_read_serial();
void task_read_distance();
void task_lcd_status();
void task_blink_led();
void task_read_adc();
void task_read_switch();
void task_sound();
void i2s_event_handler(void* arg, cyhal_i2s_event_t event) ;


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
    cyhal_i2s_register_callback(&i2s, &i2s_event_handler, &i2s);
    cyhal_i2s_enable_event(&i2s, CYHAL_I2S_ASYNC_TX_COMPLETE, CYHAL_ISR_PRIORITY_DEFAULT, true);
    cyhal_i2s_set_async_mode(&i2s, CYHAL_ASYNC_DMA, CYHAL_DMA_PRIORITY_DEFAULT);
    cyhal_i2s_write_async(&i2s, _362Hz, _362Hz_size);
    cyhal_i2s_start_tx(&i2s);


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
        1,
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

void copy_audio(const int16_t *array, int size) {
    int divisor;
    if (curLcdMode == 1) {
        int adcmap = adc1_value / 9411; // map adc to 0 - 255
        divisor = ((range_sensor2 + adcmap) / 2) / 26 + 1; // map to a value between 1 and 10
    } else {
        divisor = range_sensor2 / 26 + 1;
    }
    
    for (int i = 0; i < size; i++) {
        curWave[i] = array[i] / divisor;
    }
    curWaveSize = size;
}

void i2s_event_handler(void* arg, cyhal_i2s_event_t event) {
        cyhal_i2s_t* i2s = (cyhal_i2s_t*)arg;
    if (0u != (event & CYHAL_I2S_ASYNC_TX_COMPLETE)) {
        int compare;
#define SOMETHING 8
        if (curLcdMode == 1) {
            int adcmap = adc0_value / 9411;
            compare = ((range_sensor1 + adcmap) / 2) / 8;
        } else {
            compare = range_sensor1 / 8;
        }
        switch (compare) {
        case 0:
            copy_audio(_162Hz, _162Hz_size);
            break;
        case 1:
            copy_audio(_172Hz, _172Hz_size);
            break;
        case 2:
            copy_audio(_182Hz, _182Hz_size);
            break;
        case 3:
            copy_audio(_192Hz, _192Hz_size);
            break;
        case 4:
            copy_audio(_202Hz, _202Hz_size);
            break;
        case 5:
            copy_audio(_212Hz, _212Hz_size);
            break;
        case 6:
            copy_audio(_222Hz, _222Hz_size);
            break;
        case 7:
            copy_audio(_232Hz, _232Hz_size);
            break;
        case 8:
            copy_audio(_242Hz, _242Hz_size);
            break;
        case 9:
            copy_audio(_252Hz, _252Hz_size);
            break;
        case 10:
            copy_audio(_262Hz, _262Hz_size);
            break;
        case 11:
            copy_audio(_272Hz, _272Hz_size);
            break;
        case 12:
            copy_audio(_282Hz, _282Hz_size);
            break;
        case 13:
            copy_audio(_292Hz, _292Hz_size);
            break;
        case 14:
            copy_audio(_302Hz, _302Hz_size);
            break;
        case 15:
            copy_audio(_312Hz, _312Hz_size);
            break;
        case 16:
            copy_audio(_322Hz, _322Hz_size);
            break;
        case 17:
            copy_audio(_332Hz, _332Hz_size);
            break;
        case 18:
            copy_audio(_342Hz, _342Hz_size);
            break;
        case 19:
            copy_audio(_352Hz, _352Hz_size);
            break;
        case 20:
            copy_audio(_362Hz, _362Hz_size);
            break;
        case 21:
            copy_audio(_372Hz, _372Hz_size);
            break;
        case 22:
            copy_audio(_382Hz, _382Hz_size);
            break;
        case 23:
            copy_audio(_392Hz, _392Hz_size);
            break;
        case 24:
            copy_audio(_402Hz, _402Hz_size);
            break;
        case 25:
            copy_audio(_412Hz, _412Hz_size);
            break;
        case 26:
            copy_audio(_422Hz, _422Hz_size);
            break;
        case 27:
            copy_audio(_432Hz, _432Hz_size);
            break;
        case 28:
            copy_audio(_442Hz, _442Hz_size);
            break;
        case 29:
            copy_audio(_452Hz, _452Hz_size);
            break;
        default:
            copy_audio(_462Hz, _462Hz_size);
            break;
        }
        cyhal_i2s_write_async(i2s, curWave, curWaveSize);
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
                writeString("ADC mode");
            }
        } else if (cyhal_gpio_read(BUTTON_PIN) == true && buttonPressed == 1) {
            buttonPressed ^= 1;
        }
        if (cyhal_gpio_read(SWITCH_PIN) == true) { // having true = off
            noBacklight();
            cyhal_i2s_enable_event(&i2s, CYHAL_I2S_ASYNC_TX_COMPLETE, CYHAL_ISR_PRIORITY_DEFAULT, false);
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
            // sprintf(lcdstring, "User Mode Task");
            // writeStringWithoutClear(1, 0, lcdstring, 4);
        }
    }
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
            printf("If you're hearing a sound, then FreeRTOS and I2S are working\n");
        }
        cInputIndex = 0;
        free(args);
    }
}