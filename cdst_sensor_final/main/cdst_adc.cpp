/**
 * ADC helper
 * reads from ECG analog out, outputs 12-bit int value.
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_system.h"
#include "esp_adc_cal.h"
#include "cdst_adc.h"

uint32_t adc_read(void) {
	//Init ADC and Characteristics
    esp_adc_cal_characteristics_t characteristics;
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_TEST_CHANNEL, ADC_ATTEN_DB_11);
    esp_adc_cal_get_characteristics(V_REF, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, &characteristics);
    uint32_t voltage;

    //voltage = adc1_get_raw(ADC1_TEST_CHANNEL);
    voltage = adc1_to_voltage(ADC1_TEST_CHANNEL, &characteristics);

    /* sets a time delay to simulate 200Hz sampling rate */
    //vTaskDelay(pdMS_TO_TICKS(5));

    return voltage;
}