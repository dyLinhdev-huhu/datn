#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "SDS011.h"
#include "PubSubClient.h"

float p10, p25;
int error;

SDS011 my_sds;
WiFiClient espClient;
PubSubClient client(espClient);

const char* ssid = "Nha So 8";
const char* password = "66668888";
const char* mqtt_server = "broker.hivemq.com";
int startFlag = 0;

#define enaTopic "datn/dyLinh_dev/dustSensor/enable"

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
    if(strstr(topic, enaTopic) != NULL)
    {
        if(strstr((char*)payload, "START"))
        {
            startFlag = 1;
        }
        if(strstr((char*)payload, "STOP"))
        {
            startFlag = 0;
        }
        
    }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe(enaTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
	my_sds.begin(D1, D2); //RX, TX
	Serial.begin(9600);
  	setup_wifi();
  	client.setServer(mqtt_server, 1883);
  	client.setCallback(callback);
}

void loop() {
	if (!client.connected()) {
		reconnect();
	}
	client.loop();
	if(startFlag)
	{
		error = my_sds.read(&p25, &p10);
		if (!error) {
			// Serial.println("P2.5: " + String(p25));
			Serial.println("P10:  " + String(p10));
		}
		delay(10000);
	}
}