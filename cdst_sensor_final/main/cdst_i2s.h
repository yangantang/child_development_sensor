/*
* We will be using the SPH0645LM4H-B MEMs microphone breakout module.
* Purchase link: https://www.adafruit.com/product/3421
*
* It can operate up to 64k sample/s. However, we will just use 8k
* sample/s. There 2 I2S peripherals available on the ESP32 board:
* I2S0 and I2S1. We will use I2S0.
*/
#define SAMPLE_RATE     (4000)
#define I2S_NUM         (0)

/*
 * - Port: UART1
 * - Receive (Rx) buffer: on
 * - Transmit (Tx) buffer: off
 * - Flow control: off
 * - Event queue: off
 * - Pin assignment: see defines below
 */
#define ECHO_TEST_TXD  (GPIO_NUM_14)
#define ECHO_TEST_RXD  (GPIO_NUM_12)
#define ECHO_TEST_RTS  (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS  (UART_PIN_NO_CHANGE)

#define BUF_SIZE (1024)

/* initialize i2s drivers */
void i2s_init();

/* read 1 i2s sample */
int i2s_read();