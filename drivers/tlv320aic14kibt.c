#include "tlv320aic14kibt.h"
#include "i2c.h"

void tlv320aic14kibt_convert(){
    uint8_t buffer[1];
    buffer[0] = TLV320AIC14KIBT_ACCEPT_TRANS;
    cyhal_i2c_master_write(&i2c_master_obj, TLV320AIC14KIBT_I2C_ADDR, buffer, 1, 0, false);
}
