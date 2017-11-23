/**
 * Create a new BLE server. Then create a wifi connection.
 * Once receiving fom client, publish through MQTT to AWS.
 */
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include "esp_log.h"
#include <string>
#include <Task.h>
#include <WiFi.h>
#include <WiFiEventHandler.h>
#include <AWS.h>
#include "sdkconfig.h"
#include "common.h"

using namespace std;

static char LOG_TAG[] = "CDSTServer";

/* AWS configs */
char custom_endpoint[] = {"a23zuivoie0e9u.iot.us-east-2.amazonaws.com"};
char clientID[] = AWS_IOT_MQTT_CLIENT_ID;
static WiFi *wifi;
AWS *aws;

class MyCallbacks: public BLECharacteristicCallbacks {
	void onWrite(BLECharacteristic *pCharacteristic) {
		string value = pCharacteristic->getValue();
		string ecgCheck = "a";
		QoS qos = QOS0;

		if(value.length() > 0) {
			/* checking if ecg data */
			if(value.substr(0, 1) == ecgCheck) {
				/* publish to AWS IoT */
				aws->publish("test", value, qos);
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
		/* AWS client establishes connection then publishes.	
		 * Create AWS class def and intialize mqtt connection.
		 */
		ESP_LOGD(LOG_TAG, "Connecting to AWS REST http");
		uint16_t port = 8883;

		aws = new AWS();
		aws->init(custom_endpoint, port);
		ESP_LOGD(LOG_TAG, "AWS client initialized");
		aws->connect(clientID, 5);
		ESP_LOGD(LOG_TAG, "AWS client connected");

		/* create BLE server and initialize callback
		 * that receives from client
		 */
		ESP_LOGD(LOG_TAG, "Starting BLE!");

		BLEDevice::init("ESP32BT");

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

/* Event handler that creats AWS task */
class wiFiEventHandler: public WiFiEventHandler {
    esp_err_t staGotIp(system_event_sta_got_ip_t event_sta_got_ip) {
		ESP_LOGD(LOG_TAG, "MyWiFiEventHandler: Got IP");

		/* set up BLE server */
		MainBLEServer* pMainBleServer = new MainBLEServer();
		pMainBleServer->setStackSize(18000);
		pMainBleServer->start();

		return ESP_OK;
	}
};

void SampleServer(void)
{
	/* setting up event handler and wifi struct */
	wiFiEventHandler *eventHandler = new wiFiEventHandler();
	wifi = new WiFi();
	wifi->setWifiEventHandler(eventHandler);
	wifi->connectAP(EAP_USERNAME, EAP_PASSWORD);

} // app_main