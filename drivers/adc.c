#include "adc.h"

cyhal_adc_t adc_obj;
cyhal_adc_channel_t adc_chan_0_obj, adc_chan_1_obj;

void adc_init(){
    cy_rslt_t rslt;
    /* ADC conversion result. */
    int32_t adc_out;
    /* Initialize ADC. The ADC block which can connect to pin 10[0] is selected */
    rslt = cyhal_adc_init(&adc_obj, P10_5, NULL);
    /* Initialize ADC channel, allocate channel number 0 to pin 10[0] as this is the first channel initialized */
    /* Default ADC configuration */
    const cyhal_adc_config_t adc_config = {
            .continuous_scanning=true, // Continuous Scanning is disabled
            .average_count=1,           // Average count disabled
            .vref=CYHAL_ADC_REF_VDDA,   // VREF for Single ended channel set to VDDA
            .vneg=CYHAL_ADC_VNEG_VSSA,  // VNEG for Single ended channel set to VSSA
            .resolution = 12u,          // 12-bit resolution
            .ext_vref = NC,             // No connection
            .bypass_pin = NC };       // No connection
    rslt = cyhal_adc_channel_init_diff(&adc_chan_0_obj, &adc_obj, P10_5, CYHAL_ADC_VNEG, &adc_config);
    rslt = cyhal_adc_channel_init_diff(&adc_chan_1_obj, &adc_obj, P10_4, CYHAL_ADC_VNEG, &adc_config);
}

int32_t read_adc(cyhal_adc_channel_t channel){
    return cyhal_adc_read_uv(&channel);
}
