/*
 * console.h
 *
 *  Created on: Jan 21, 2022
 *      Author: Joe Krachey
 */

#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <FreeRTOS.h>
#include <FreeRTOSConfig.h>
#include <task.h>
#include <semphr.h>
#include <event_groups.h>

extern TaskHandle_t console_task;

#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"


#define PIN_CONSOLE_TX 	P6_5
#define PIN_CONSOLE_RX	P6_4


#define DEBUG_MESSAGE_MAX_LEN   (100u)
#define INT_PRIORITY_CONSOLE	3


extern char pcInputString[];
extern int8_t cInputIndex;

/* Public Function API */
void console_init(void);


#endif /* CONSOLE_H_ */
