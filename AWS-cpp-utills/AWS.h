/*
 * AWS.h
 *
 *  Created on: May 31, 2017
 *      Author: kolban
 */

#ifndef COMPONENTS_AWS_H_
#define COMPONENTS_AWS_H_
#include "sdkconfig.h"
#ifdef CONFIG_AWS_IOT_SDK

#include <aws_iot_mqtt_client_interface.h>
#include <aws_iot_mqtt_client.h>
#include <string>
#include <FreeRTOS.h>

/**
 * @brief AWS IoT access.
 */
class AWS {
public:
	AWS();
	virtual ~AWS();

	void connect(char* clientId, uint16_t length);
	void disconnect();
	void init(char* host, uint16_t port=CONFIG_AWS_IOT_MQTT_PORT);
	void publish(std::string topic, std::string payload, QoS qos = QOS0);
	void subscribe(std::string topic);
	void unsubscribe(std::string topic);

private:
	AWS_IoT_Client m_client;

	FreeRTOS::Semaphore m_semaphoreRWEvt = FreeRTOS::Semaphore("RWEvt");
};
#endif // CONFIG_AWS_IOT_SDK
#endif /* COMPONENTS_AWS_H_ */
