/**
 * CDST Sensor Firmware
 * reads from mic and ECG, processes it then sends over bluetooth 
 * to main hub.
 */

extern "C" {
	void app_main(void);
}


// The list of sample entry points.
void SampleClient(void);
void SampleServer(void);

void app_main(void) {

	/* Calls BLE helper functions to transmit */
	SampleClient();

} // app_main
