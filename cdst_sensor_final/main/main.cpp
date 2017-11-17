/**
 * CDST Sensor Firmware
 * reads from mic and ECG, processes it then sends over bluetooth 
 * to main hub.
 */
extern "C" {
	void app_main(void);
}

#include "cdst_i2s.h"

// The list of sample entry points.
void SampleClient(void);
void SampleServer(void);

void app_main(void) {
	/* Initialize I2S */
	i2s_init();

	/* Calls BLE helper functions to transmit */
	SampleClient();

} // app_main