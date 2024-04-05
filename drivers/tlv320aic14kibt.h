/*
 * adc.h
 *
 *  Created on: April 2, 2024
 *      Author: Maxwell Martin
 */

#ifndef TLV320AIC14KIBT_H_
#define TLV320AIC14KIBT_H_

#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"

extern cyhal_adc_t adc_obj;
extern cyhal_adc_channel_t adc_chan_0_obj, adc_chan_1_obj;

/* Public API */
void adc_init();

int32_t read_adc();

#endif /* TLV320AIC14KIBT_H_ */
