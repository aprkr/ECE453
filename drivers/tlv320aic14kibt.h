/*
 * TLV320AIC14KIBT.h
 *
 *  Created on: April 2, 2024
 *      Author: Maxwell Martin
 */

#ifndef TLV320AIC14KIBT_H_
#define TLV320AIC14KIBT_H_

#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"

/* device addr */
#define TLV320AIC14KIBT_I2C_ADDR            0x40
/* control registers */
#define TLV320AIC14KIBT_CONTROL_REG_1       0x01
#define TLV320AIC14KIBT_CONTROL_REG_2       0x02
#define TLV320AIC14KIBT_CONTROL_REG_3       0x03
#define TLV320AIC14KIBT_CONTROL_REG_4       0x04
#define TLV320AIC14KIBT_CONTROL_REG_5       0x05
#define TLV320AIC14KIBT_CONTROL_REG_6       0x06
/* commands */
#define TLV320AIC14KIBT_ACCEPT_TRANS        0x40


/* Public API */
void tlv320aic14kibt_init();

#endif /* TLV320AIC14KIBT_H_ */
