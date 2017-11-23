/*
 * AWS.cpp
 *
 *  Created on: May 31, 2017
 *      Author: kolban
 */
#include "sdkconfig.h"
#ifdef CONFIG_AWS_IOT_SDK
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include <esp_log.h>
#include "AWS.h"
#include <string>

static char tag[] = "AWS";

const char rootCA[] = {"-----BEGIN CERTIFICATE-----\n\
MIIE0zCCA7ugAwIBAgIQGNrRniZ96LtKIVjNzGs7SjANBgkqhkiG9w0BAQUFADCB\n\
yjELMAkGA1UEBhMCVVMxFzAVBgNVBAoTDlZlcmlTaWduLCBJbmMuMR8wHQYDVQQL\n\
ExZWZXJpU2lnbiBUcnVzdCBOZXR3b3JrMTowOAYDVQQLEzEoYykgMjAwNiBWZXJp\n\
U2lnbiwgSW5jLiAtIEZvciBhdXRob3JpemVkIHVzZSBvbmx5MUUwQwYDVQQDEzxW\n\
ZXJpU2lnbiBDbGFzcyAzIFB1YmxpYyBQcmltYXJ5IENlcnRpZmljYXRpb24gQXV0\n\
aG9yaXR5IC0gRzUwHhcNMDYxMTA4MDAwMDAwWhcNMzYwNzE2MjM1OTU5WjCByjEL\n\
MAkGA1UEBhMCVVMxFzAVBgNVBAoTDlZlcmlTaWduLCBJbmMuMR8wHQYDVQQLExZW\n\
ZXJpU2lnbiBUcnVzdCBOZXR3b3JrMTowOAYDVQQLEzEoYykgMjAwNiBWZXJpU2ln\n\
biwgSW5jLiAtIEZvciBhdXRob3JpemVkIHVzZSBvbmx5MUUwQwYDVQQDEzxWZXJp\n\
U2lnbiBDbGFzcyAzIFB1YmxpYyBQcmltYXJ5IENlcnRpZmljYXRpb24gQXV0aG9y\n\
aXR5IC0gRzUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCvJAgIKXo1\n\
nmAMqudLO07cfLw8RRy7K+D+KQL5VwijZIUVJ/XxrcgxiV0i6CqqpkKzj/i5Vbex\n\
t0uz/o9+B1fs70PbZmIVYc9gDaTY3vjgw2IIPVQT60nKWVSFJuUrjxuf6/WhkcIz\n\
SdhDY2pSS9KP6HBRTdGJaXvHcPaz3BJ023tdS1bTlr8Vd6Gw9KIl8q8ckmcY5fQG\n\
BO+QueQA5N06tRn/Arr0PO7gi+s3i+z016zy9vA9r911kTMZHRxAy3QkGSGT2RT+\n\
rCpSx4/VBEnkjWNHiDxpg8v+R70rfk/Fla4OndTRQ8Bnc+MUCH7lP59zuDMKz10/\n\
NIeWiu5T6CUVAgMBAAGjgbIwga8wDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8E\n\
BAMCAQYwbQYIKwYBBQUHAQwEYTBfoV2gWzBZMFcwVRYJaW1hZ2UvZ2lmMCEwHzAH\n\
BgUrDgMCGgQUj+XTGoasjY5rw8+AatRIGCx7GS4wJRYjaHR0cDovL2xvZ28udmVy\n\
aXNpZ24uY29tL3ZzbG9nby5naWYwHQYDVR0OBBYEFH/TZafC3ey78DAJ80M5+gKv\n\
MzEzMA0GCSqGSIb3DQEBBQUAA4IBAQCTJEowX2LP2BqYLz3q3JktvXf2pXkiOOzE\n\
p6B4Eq1iDkVwZMXnl2YtmAl+X6/WzChl8gGqCBpH3vn5fJJaCGkgDdk+bW48DW7Y\n\
5gaRQBi5+MHt39tBquCWIMnNZBU4gcmU7qKEKQsTb47bDN0lAtukixlE0kF6BWlK\n\
WE9gyn6CagsCqiUXObXbf+eEZSqVir2G3l6BFoMtEMze/aiCKm0oHw0LxOXnGiYZ\n\
4fQRbxC1lfznQgUy286dUV4otp6F01vvpX1FQHKOtw5rDgb7MzVIcbidJ4vEZV8N\n\
hnacRHr2lVz2XTIIM6RUthg/aFzyQkqFOFSDX9HoLPKsEdao7WNq\n\
-----END CERTIFICATE-----\n"};

const char devicePrivateKey[] = {"-----BEGIN RSA PRIVATE KEY-----\n\
MIIEowIBAAKCAQEAj2ECHXjIXuW9sEdHJTMUoUb5gAuNGZh8zzLt/XLKypmFZuWx\n\
xMgG/HK9sqJUnLO+vwg1RxoOCCBDLo0e00ABTzIgEoBJbi4u8g7O7zCMLvVsakWd\n\
tljCem39tLTeDI+9gX/ON0HSwxQHl5wN+uXWaxj1dbtO2nc+oax4py6F6h8GPlXf\n\
zRhBX6HBt9sx3fefuAFwNZnZYDSSZaYa7cIi0YQfzj/PiEe/+rrKODBoo539fp0f\n\
bkaSgNo5oiP4k/ReTtPUkzebVbxNdGes1wA52Xsgo7EzUXAnQ6fWsbVPl2lBzcm3\n\
puVjTkmO2qvvOIAlfM9vhHo6YSUT1NI46Bd7uwIDAQABAoIBAEd4N7GxL0/+fRuN\n\
rC0i1Tm/Q0dMer0zgaNW48Leoa9z70dnX8GTY8ga+ew9dqfWAVMAKG6U0kHCszpV\n\
JfRvhzeO6OXQBMpjQwttzkD0liwiMjSXFxJ6FBcrFLgrIDRqwi/3ITfyG7kaFt6/\n\
70gNoWApRNG22IuVvSah7lKHokEdDrJYTTMJ5Vxv8NM8K50I0uI9WRWJC8GTwBt0\n\
uTUf1uucGbrNa3wN+RSscTYn3+PdQJeiKKDR7ijitN/KzvOlOK71KmB5/KScczuA\n\
49AfKjk7vJWvbDvnBkilN+AAP7iBXN40z5Qwxh4rB6QnqTTNIJlHUqzEgq2SQdKC\n\
4WDF52ECgYEAyF3uqv+cpC+YKQGYPj2Wmk8t/Z9r11lWTw1i/3u2YkWv4RCoRiSD\n\
M+18LpalBBaNcJ+b2F8iH6kNc4OWbSES4SGm6a6RUqPwueWqtyFn3+xMv4Tl3EDi\n\
vQ6BjZrufsdS6QUHmpuX8zvfzDxHomuryZrSHBClPftJammYO9l2Nl0CgYEAtzBf\n\
r9bszNCUaqPqoO1cSeAwPG4acCeo3kLgvUTGSjB20xOsfS9IoPhLMbGVwI12k6qK\n\
yfd0063SPt/lOna0IRTeOr/Aux396zNNR9O573k33OSl5fmM7XRYp2YEeYplvA8t\n\
xZT4SO3y+jNyJOS6XMXfhEHHgvrWLlmKxGMMqPcCgYAULBDJOm7EwN3q0vRG3+B3\n\
rJ0OuawbxCzCTLC40/WqsyVf3VNzrF1hLvmfnx8B6FtZCVihUgKXeWeXR5ovZnMG\n\
I2Mwh5it3z9vF8Y2JbDVhG69oV2a7RVmvz0fyodUUDDysMTxNdT4cgC4CSGVIo3r\n\
ACFDHhHkyZW8luDytZQHKQKBgQCyO6qd1ytGi5obt6hNMyQb7LiB00592NiAh+lJ\n\
vWSAQm8aJ4Yntp2foHTowLPHSNwaCFBI+S791Z0993F8VIoltbwSxQasCB5JfPyv\n\
6dx6lyI3FXAkNY1ca1rmZGfuKS2ar9oE51A7BOjnbq6cHr8EDLHQaH/JfGxvZEeT\n\
lBt5OQKBgGkXlQtnbu0Tckqq8HqsNF4hzuaBFF+00tAZNEME4E1bVbyGUgnhatXr\n\
6UFocP6uwmC8VdRXSYC01PA9Y5duqYc0JU3fb/CtFJ+GN6boxai/00+89kGr2hb1\n\
I73/MBnEh5joCVDoab5bEb4WAs8CpiIy3aEnha7eqWeDrLBN1fFF\n\
-----END RSA PRIVATE KEY-----\n"};

const char deviceCert[] = {"-----BEGIN CERTIFICATE-----\n\
MIIDWjCCAkKgAwIBAgIVAO8oCC0DnfkyjucZZV2aiS6d60e1MA0GCSqGSIb3DQEB\n\
CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t\n\
IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0xNzExMjAyMzQ0\n\
MzNaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh\n\
dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCPYQIdeMhe5b2wR0cl\n\
MxShRvmAC40ZmHzPMu39csrKmYVm5bHEyAb8cr2yolScs76/CDVHGg4IIEMujR7T\n\
QAFPMiASgEluLi7yDs7vMIwu9WxqRZ22WMJ6bf20tN4Mj72Bf843QdLDFAeXnA36\n\
5dZrGPV1u07adz6hrHinLoXqHwY+Vd/NGEFfocG32zHd95+4AXA1mdlgNJJlphrt\n\
wiLRhB/OP8+IR7/6uso4MGijnf1+nR9uRpKA2jmiI/iT9F5O09STN5tVvE10Z6zX\n\
ADnZeyCjsTNRcCdDp9axtU+XaUHNybem5WNOSY7aq+84gCV8z2+EejphJRPU0jjo\n\
F3u7AgMBAAGjYDBeMB8GA1UdIwQYMBaAFKU3rLq5EwYZLugTY9hQgZ9COoWfMB0G\n\
A1UdDgQWBBQ3/aYTj6ZMkS55CG0zyR71nAi7YTAMBgNVHRMBAf8EAjAAMA4GA1Ud\n\
DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAMzOGVf1TzPYnSNjtwmFHIE50\n\
YXsvXseBVCxpT8L9so/Y/x88a2JfxZNmR1PAp2DAy0Qb8VoqcaIaXeZ+P+8g2LdB\n\
6GYGiPejZF6B1iLXqW5hi4zm51FcP/L3P5mrQ+Zqk7ztVyEJny6Sq3k6H1ec56uu\n\
cKWaZ7yd3VqyLivbWs0ur6JaxDA0OIX+eDSPnI35P9kc+n9fpKMLBE2mm/98nl8w\n\
bfdiTS3eNPj2CR4i6LNgPdXW3LPWkT/fJOg8CRFxldfybGiptAxXnQgGVXkbZiZ6\n\
rGgdScU83pcNL8TZ+F3Nal3f/0JED1LrPgO2o3WmzI9YKjfVhzvn5atik2gaMw==\n\
-----END CERTIFICATE-----\n"}
;

AWS::AWS() {
}


AWS::~AWS() {
}


/**
 * @brief Connect to the AWS IoT service.
 * Connect to the AWT IoT service.
 * @param [in] The client id.
 * @return N/A.
 */
void AWS::connect(char* clientId, uint16_t length) {
	m_semaphoreRWEvt.take("connect");

	IoT_Client_Connect_Params connectParams = iotClientConnectParamsDefault;
	connectParams.keepAliveIntervalInSec = 10;
	connectParams.isCleanSession   = true;
	connectParams.MQTTVersion      = MQTT_3_1_1;
	connectParams.isWillMsgPresent = false;
	connectParams.pClientID        = (char *) clientId;
	connectParams.clientIDLen      = length;

	IoT_Error_t err = ::aws_iot_mqtt_connect(&m_client, &connectParams);
	if (err != SUCCESS) {
		ESP_LOGD(tag, "aws_iot_mqtt_connect: error=%d", err);
	}

	m_semaphoreRWEvt.give();
} // connect


/**
 * @brief Disconnect a previously formed connection.
 * Disconnect from the AWT IoT service.
 * @return N/A.
 */
void AWS::disconnect() {
	IoT_Error_t err = ::aws_iot_mqtt_disconnect(&m_client);
	if (err != SUCCESS) {
		ESP_LOGD(tag, "aws_iot_mqtt_disconnect: error=%d", err);
	}
} // disconnect


/**
 * @brief Initialize our connection.
 * @param [in] host The the host of the AWS IoT service.
 * @param [in] port The port number of the AWS IoT service.
 * @return N/A.
 */
void AWS::init(char* host, uint16_t port) {
	m_semaphoreRWEvt.take("init");

	IoT_Client_Init_Params initParams = iotClientInitParamsDefault;

	initParams.pHostURL                  = (char *) host;
	initParams.port                      = port;
	initParams.pRootCALocation           = (const char *) rootCA;
	initParams.pDeviceCertLocation       = (const char *) deviceCert;
	initParams.pDevicePrivateKeyLocation = (const char *) devicePrivateKey;
	initParams.mqttCommandTimeout_ms     = 20000;
	initParams.tlsHandshakeTimeout_ms    = 20000;
	initParams.isSSLHostnameVerify       = true;

	IoT_Error_t err = ::aws_iot_mqtt_init(&m_client, &initParams);
	if (err != SUCCESS) {
		ESP_LOGD(tag, "aws_iot_mqtt_init: error=%d", err);
	}

	m_semaphoreRWEvt.give();
} // init


/**
 * @brief Publish a message.
 * Publish a message on the given topic.
 *
 * @param [in] topic The topic against which we wish to publish.
 * @param [in] payload The payload of the message we wish to publish.
 * @param [in] qos The quality of service for the publish.
 * @return N/A.
 */
void AWS::publish(std::string topic, std::string payload, QoS qos) {
	IoT_Publish_Message_Params message;
	message.payload = (void *)payload.data();
	message.payloadLen = payload.length();
	message.qos = qos;
	message.isRetained = 0;
	IoT_Error_t err = ::aws_iot_mqtt_publish(&m_client, topic.c_str(), topic.length(), &message);
	if (err != SUCCESS) {
		ESP_LOGD(tag, "aws_iot_mqtt_publish: error=%d", err);
	}
} // publish


/**
 * @brief Subscribe to a topic.
 * Future publications on this topic will be delivered to us.
 * @param [in] topic The topic against which we wish to subscribe.
 * @return N/A.
 */
void AWS::subscribe(std::string topic) {
	pApplicationHandler_t iot_subscribe_callback_handler = nullptr;
	IoT_Error_t err = ::aws_iot_mqtt_subscribe(&m_client, topic.c_str(), topic.length(), QOS0, iot_subscribe_callback_handler, NULL);
	if (err != SUCCESS) {
		ESP_LOGD(tag, "aws_iot_mqtt_subscribe: error=%d", err);
	}
} // subscribe


/**
 * @brief Un-subscribe from a previous subscription.
 * Further publications on this topic will no longer be delivered to us.
 * @param [in] topic The topic to un-subscribe from.
 * @return N/A.
 */
void AWS::unsubscribe(std::string topic) {
	IoT_Error_t err = ::aws_iot_mqtt_unsubscribe(&m_client, topic.c_str(), topic.length());
	if (err != SUCCESS) {
		ESP_LOGD(tag, "aws_iot_mqtt_unsubscribe: error=%d", err);
	}
} // unsubscribe
#endif // CONFIG_AWS_IOT_SDK
