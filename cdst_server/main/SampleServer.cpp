/**
 * Create a new BLE server.
 */
#include "BLEDevice.h"
#include "BLEServer.h"
#include "BLEUtils.h"
#include "BLE2902.h"
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include "esp_log.h"
#include <string>
#include <Task.h>
#include "sdkconfig.h"

static char LOG_TAG[] = "CDSTServer";

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
// The remote service we wish to connect to.
static BLEUUID serviceUUID("91bad492-b950-4226-aa2b-4ede9fa42f59");
// The characteristic of the remote service we are interested in.
static BLEUUID    charUUID("0d563a58-196a-48ce-ace2-dfec78acc814");

class MyCallbacks: public BLECharacteristicCallbacks {
	void onWrite(BLECharacteristic *pCharacteristic) {
		std::string value = pCharacteristic->getValue();
		if (value.length() > 0) {
				char buf[value.length()];
				int i;

				/* Fix for race condition */
				FreeRTOS::sleep(0.001);

				for(i = 0; i < value.length(); i++) {
					buf[i] = value[i];
					ets_write_char_uart(buf[i]);
				}
				ets_printf("\n");
				// ESP_LOGD(LOG_TAG, "*********");
				// ESP_LOGD(LOG_TAG, "New value: %s", buf);
				// ESP_LOGD(LOG_TAG, "*********");
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

		pCharacteristic->setCallbacks(new MyCallbacks());

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
	//esp_log_level_set("*", ESP_LOG_DEBUG);
	MainBLEServer* pMainBleServer = new MainBLEServer();
	pMainBleServer->setStackSize(20000);
	pMainBleServer->start();

} // app_main