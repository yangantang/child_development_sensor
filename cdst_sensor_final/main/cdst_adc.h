/**
 * ADC helper
 * reads from ECG analog out, outputs 12-bit int value.
 */

/*Note: Different ESP32 modules may have different reference voltages varying from
 * 1000mV to 1200mV. Use #define GET_VREF to route v_ref to a GPIO
 */
#define V_REF   1093
#define ADC1_TEST_CHANNEL (ADC1_CHANNEL_6)      //GPIO 34

/* adc read declaration */
uint32_t adc_read(void);