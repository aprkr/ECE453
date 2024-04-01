/*
 * i2c.h
 *
 *  Created on: Jan 21, 2022
 *      Author: Joe Krachey
 */

#ifndef I2C_H_
#define I2C_H_

#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"

#undef CONN_I2C_J300
#define CONN_I2C_J301
#undef CONN_I2C_J302

#if defined(CONN_I2C_J300)
#define PIN_MCU_SCL			P9_0
#define PIN_MCU_SDA			P9_1
#elif defined(CONN_I2C_J301)
#define PIN_MCU_SCL			P10_0
#define PIN_MCU_SDA			P10_1
#elif defined(CONN_I2C_J302)
#define PIN_MCU_SCL			P5_0
#define PIN_MCU_SDA			P5_1
#else
#error "MUST DEFINE AN I2C CONNECTOR"
#endif

/* Macros */
#define I2C_MASTER_FREQUENCY 100000u

/* Public Global Variables */
extern cyhal_i2c_t i2c_master_obj;

/* Public API */

/** Initialize the I2C bus to the specified module site
 *
 * @param - None
 */
cy_rslt_t i2c_init(void);

#endif /* I2C_H_ */
