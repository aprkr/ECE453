/*
 * adc.h
 *
 *  Created on: April 2, 2024
 *      Author: Maxwell Martin
 */

#ifndef I2S_H_
#define I2S_H_

#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"

/* Master I2C Settings */
#define MI2C_TIMEOUT_MS     10u         /* in ms */
/* Note that we are not able to achieve the desired frequency, so we round up
*  the frequency values to avoid mismatches */
/* Master Clock (MCLK) Settings */
#define MCLK_FREQ_HZ        4083000u    /* in Hz (Ideally 4.096 MHz) */
#define MCLK_DUTY_CYCLE     50.0f       /* in %  */
/* Clock Settings */
#define AUDIO_SYS_CLOCK_HZ  98000000u   /* in Hz (Ideally 98.304 MHz) */
/* PWM MCLK Pin */
#define MCLK_PIN            P5_0
/* HFCLK1 Clock Divider */
#define HFCLK1_CLK_DIVIDER  4u

extern cyhal_pwm_t mclk_pwm;
extern cyhal_i2s_t i2s;
extern cyhal_clock_t audio_clock;
extern cyhal_clock_t pll_clock;
extern cyhal_clock_t fll_clock;
extern cyhal_clock_t system_clock;

/* Public API */
void i2s_init();

#endif /* I2S_H_ */
