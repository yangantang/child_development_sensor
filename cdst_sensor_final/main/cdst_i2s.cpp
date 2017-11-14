/* ESP32 - I2S Audio interface test
* 
* This app mainly receives audio data through i2s interface, then sends
* it to the UART port. 
* By connecting the USB from a PC to the UART port on the board, we can 
* run a program on the host PC to collect these data and save them for 
* playback later.
*
*/
#include <esp_log.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s.h"
#include "driver/uart.h"
#include "esp_system.h"
#include "cdst_i2s.h"

static const char* LOG_TAG = "CDSTClient";

 /* CONNFIGURE THE I2S PORT
* Mode:             Master, and receiver (RX)
* Sampling rate:    8kHz
* bits/sample:      32-bits. Datasheet states 24-bit, but somehow
                    only 32-bits works, and online examples
                    conforms this. Out of the 24-bits, only the
                    18 MSBs are actual data (2's-complement).
* Channels:         2 (can actually hook up 2 microphones L/R)
* Buffer Length:    64 (Size of each buffer in bytes, simply followed
                    examples online)
* Buffer Count:     8 (# of buffers)
* use_apll:         Yes. (Not necessary, it just makes the clock more
                    more accurate)
* Alloc Flags:      ESP interupt level 1.
*/
i2s_config_t i2s_config = {
    .mode = I2S_MODE_MASTER | I2S_MODE_RX,                                  
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = 32,                                              
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,                           
    .communication_format = I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB | I2S_COMM_FORMAT_PCM,
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = 1,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1                                
};

/* CONFIGURE I2S PINS
* BCK (BCLK):   pin 26
* WS (LRSL):    pin 25
* data out:     not used
* data in:      pin 13 (Dout from microphone)
*/
i2s_pin_config_t pin_config = {
    .bck_io_num = 26,
    .ws_io_num = 25,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = 13                                                       
};

/* CONFIGURE UART INTERFACE
* Parity bits and Flow control are disabled.
*/
uart_config_t uart_config = {
    .baud_rate = 115200,
    .data_bits = UART_DATA_8_BITS,
    .parity    = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
};

/* initialize i2s drivers */
void i2s_init() {    
    i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM, &pin_config);
    i2s_set_sample_rates(I2S_NUM, SAMPLE_RATE);
}

/* read 1 i2s sample */
int i2s_read() {
    int read, sample;
    uint8_t high_byte, low_byte, middle_byte;

    if( (read = i2s_pop_sample(I2S_NUM, (char*)&sample, 0)) == 8 ){
        sample = sample >> 16;
        sample += 1775;
    }
    
    return sample;
}
