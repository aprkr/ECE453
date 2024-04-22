#include "tlv320aic14kibt.h"
#include "i2c.h"
#include <stdio.h>
#include <stdlib.h>

void tlv320aic14kibt_init(){;
    uint8_t write_buffer[3];
    write_buffer[0] = TLV320AIC14KIBT_CONTROL_REG_1;
    write_buffer[1] = TLV320AIC14KIBT_ACCEPT_TRANS;
    write_buffer[2] = TLV320AIC14KIBT_ACCEPT_TRANS + 0x01;
    cyhal_i2c_master_write(&i2c_master_obj, TLV320AIC14KIBT_I2C_ADDR, write_buffer, 3, 0, true);
}
