/*
 * WiFi helper file
 * leverages WiFi and AWS IoT APIs to publish ECG values via MQTT
 */
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include <esp_log.h>
#include <string>
#include <Task.h>
#include <WiFi.h>
#include <WiFiEventHandler.h>
#include <AWS.h>
#include "esp_wpa2.h"
#include "sdkconfig.h"
#include "common.h"

using namespace std;

static char tag[] = "CDSTWifi";

extern "C" {
	void app_main(void);
}

/* AWS configs */
char custom_endpoint[] = {"a23zuivoie0e9u.iot.us-east-2.amazonaws.com"};
char clientID[] = AWS_IOT_MQTT_CLIENT_ID;
static WiFi *wifi;
AWS *aws;

/* Task handler for connecting to AWS IoT */
class AWSTask: public Task {
	void run(void* data) {
		/* AWS client establishes connection then publishes.	
		 * Create AWS class def and intialize mqtt connection.
		 */
		ESP_LOGD(tag, "Connecting to AWS REST http");
		uint16_t port = 8883;

		aws = new AWS();
		aws->init(custom_endpoint, port);
		ESP_LOGD(tag, "AWS client initialized");
		aws->connect(clientID, 5);
		ESP_LOGD(tag, "AWS client connected");
		
		// ESP_LOGD(tag, "AWS client disconnecting...");
		// aws->disconnect();
		// ESP_LOGD(tag, "AWS client disconnected initialized");
	}
};

static AWSTask *awsTask;

/* Event handler that creats AWS task */
class wiFiEventHandler: public WiFiEventHandler {
    esp_err_t staGotIp(system_event_sta_got_ip_t event_sta_got_ip) {
		ESP_LOGD(tag, "MyWiFiEventHandler: Got IP");

		awsTask = new AWSTask();
		awsTask->setStackSize(5000);
		awsTask->start();

		return ESP_OK;
	}
};

void cdstWifiInit(void) {
	ESP_LOGD(tag, "WiFi starting");
	
	/* setting up event handler and wifi struct */
	wiFiEventHandler *eventHandler = new wiFiEventHandler();
	wifi = new WiFi();
	wifi->setWifiEventHandler(eventHandler);
	wifi->connectAP(EAP_USERNAME, EAP_PASSWORD);
}
