/**
 * Create a new BLE server. Then create a wifi connection.
 * Once receiving fom client, publish through MQTT to AWS.
 */
#include "BLEDevice.h"
#include "BLEServer.h"
#include "BLEUtils.h"
#include "BLE2902.h"
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include "esp_log.h"
#include <string>
#include <Task.h>
#include <AWS.h>
#include "sdkconfig.h"
#include "common.h"

using namespace std;

static char LOG_TAG[] = "CDSTServer";

/* wifi helper function definitions */
void cdstWifiInit(void);

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
// The remote service we wish to connect to.
static BLEUUID serviceUUID("91bad492-b950-4226-aa2b-4ede9fa42f59");
// The characteristic of the remote service we are interested in.
static BLEUUID    charUUID("0d563a58-196a-48ce-ace2-dfec78acc814");

class MyCallbacks: public BLECharacteristicCallbacks {
	void onWrite(BLECharacteristic *pCharacteristic) {
		string value = pCharacteristic->getValue();
		string ecgCheck = "a";

		if(value.length() > 0) {
			/* checking if ecg data */
			if(value.substr(0, 1) == ecgCheck) {
				/* publish to AWS IoT */
				aws->publish("test", value, QOS0);
			}

			char buf[value.length()];
			int i;

			/* Fix for race condition */
			FreeRTOS::sleep(0.001);

			for(i = 0; i < value.length(); i++) {
				buf[i] = value[i];
				ets_write_char_uart(buf[i]);
			}
			ets_printf("\n");
			}
		}
};

class MainBLEServer: public Task {
	void run(void *data) {
		ESP_LOGD(LOG_TAG, "Starting BLE!");

		BLEDevice::init("ESP32");
		BLEServer* pServer = BLEDevice::createServer();

		BLEService* pService = pServer->createService("91bad492-b950-4226-aa2b-4ede9fa42f59");

		BLECharacteristic* pCharacteristic = pService->createCharacteristic(
			BLEUUID("0d563a58-196a-48ce-ace2-dfec78acc814"),
			BLECharacteristic::PROPERTY_BROADCAST | BLECharacteristic::PROPERTY_READ  |
			BLECharacteristic::PROPERTY_NOTIFY    | BLECharacteristic::PROPERTY_WRITE |
			BLECharacteristic::PROPERTY_INDICATE
		);

		// pCharacteristic->setCallbacks(new MyCallbacks());

		pCharacteristic->setValue("What's Up World!");

		BLE2902* p2902Descriptor = new BLE2902();
		p2902Descriptor->setNotifications(true);
		pCharacteristic->addDescriptor(p2902Descriptor);

		pService->start();

		BLEAdvertising* pAdvertising = pServer->getAdvertising();
		pAdvertising->addServiceUUID(BLEUUID(pService->getUUID()));
		pAdvertising->start();

		ESP_LOGD(LOG_TAG, "Advertising started!");
		delay(1000000);
	}
};


void SampleServer(void)
{
	/* set up wifi and AWS client */
	cdstWifiInit();
	FreeRTOS::sleep(2000);

	/* set up BLE server */
	MainBLEServer* pMainBleServer = new MainBLEServer();
	pMainBleServer->setStackSize(12000);
	pMainBleServer->start();

} // app_main