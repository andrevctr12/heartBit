#include <user_config.h>
#include <SmingCore/SmingCore.h>
#include "QRSMessage.h"
#include "nodeMCU.h"

// If you want, you can define WiFi settings globally in Eclipse Environment Variables
#ifndef WIFI_SSID
	#define WIFI_SSID "heartBit" // Put you SSID and Password here
	#define WIFI_PWD "lasse2017"
#endif

// ... and/or MQTT username and password
#ifndef MQTT_USERNAME
	#define MQTT_USERNAME "heartBit"
	#define MQTT_PWD ""
#endif

// ... and/or MQTT host and port
#ifndef MQTT_HOST
	#define MQTT_HOST "test.mosquitto.org"
	#ifndef ENABLE_SSL
		#define MQTT_PORT 1883
	#else
		#define MQTT_PORT 8883
	#endif
#endif

// Forward declarations
void startMqttClient();
void onMessageReceived(String topic, String message);

Timer procTimer;

// MQTT client
// For quick check you can use: http://www.hivemq.com/demos/websocket-client/ (Connection= test.mosquitto.org:8080)
MqttClient *mqtt;
//QRSMessage object
QRSMessage *msg;

// Check for MQTT Disconnection
void checkMQTTDisconnect(TcpClient& client, bool flag){
	
	// Called whenever MQTT connection is failed.
	if (flag == true)
		Serial.println("MQTT Broker Disconnected!!");
	else
		Serial.println("MQTT Broker Unreachable!!");
	
	// Restart connection attempt after few seconds
	procTimer.initializeMs(2 * 1000, startMqttClient).start(); // every 2 seconds
}

void onMessageDelivered(uint16_t msgId, int type) {
	Serial.printf("Message with id %d and QoS %d was delivered successfully.", msgId, (type==MQTT_MSG_PUBREC? 2: 1));
}

// Publish our message
void publishMessage()
{
	if (mqtt->getConnectionState() != eTCS_Connected)
		startMqttClient(); // Auto reconnect

	msg->execute();

	mqtt->publish("/RawData", String(msg->getRawData()));
	mqtt->publish("/Data", String(msg->getData()));

	//mqtt->publishWithQoS("important/frameworks/sming", "Request Return Delivery", 1, false, onMessageDelivered); // or publishWithQoS
}

// Callback for messages, arrived from MQTT server
void onMessageReceived(String topic, String message)
{
	Serial.print(topic);
	Serial.print(":\r\n\t"); // Pretify alignment for printing
	Serial.println(message);
}

// Run MQTT client
void startMqttClient()
{
	procTimer.stop();
	if(!mqtt->setWill("last/will","The connection from this device is lost:(", 1, true)) {
		debugf("Unable to set the last will and testament. Most probably there is not enough memory on the device.");
	}
	String mqttID = "esp8266-" + String( system_get_chip_id() );
	mqtt->connect(mqttID, MQTT_USERNAME, MQTT_PWD, true);
#ifdef ENABLE_SSL
	mqtt->addSslOptions(SSL_SERVER_VERIFY_LATER);

	#include <ssl/private_key.h>
	#include <ssl/cert.h>

	mqtt->setSslClientKeyCert(default_private_key, default_private_key_len,
							  default_certificate, default_certificate_len, NULL, true);

#endif
	// Assign a disconnect callback function
	mqtt->setCompleteDelegate(checkMQTTDisconnect);
	mqtt->subscribe("heartBit/#");
}

// Will be called when WiFi station timeout was reached
void connectFail(String ssid, uint8_t ssid_len, uint8_t bssid[6], uint8_t reason)
{
	Serial.println("I'm NOT CONNECTED. Need help :(");

	// .. some you code for device configuration ..
}

void gotIP(IPAddress ip, IPAddress netmask, IPAddress gateway)
{
	// Run MQTT client
	startMqttClient();

	// Start publishing loop
	procTimer.initializeMs(20 * 1000, publishMessage).start(); // every 20 seconds
}

void init()
{
	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	Serial.systemDebugOutput(true); // Debug output to serial

	pinMode(LED_PIN, OUTPUT);
	pinMode(ECG_PIN, INPUT);
	pinMode(LEADS_OFF_PLUS_PIN, INPUT);  // Setup for leads off detection LO +
    pinMode(LEADS_OFF_MINUS_PIN, INPUT); // Setup for leads off detection LO -

	mqtt = new MqttClient(MQTT_HOST, MQTT_PORT, onMessageReceived);
	msg = new QRSMessage();

	WifiStation.config(WIFI_SSID, WIFI_PWD);
	WifiStation.enable(true);
	WifiAccessPoint.enable(false);

	// Run our method when station was connected to AP (or not connected)
	WifiEvents.onStationDisconnect(connectFail);
	WifiEvents.onStationGotIP(gotIP);
}
