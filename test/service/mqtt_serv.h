#ifndef _MQTT_SERVICE_H_
#define _MQTT_SERVICE_H_

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "PubSubClient.h"

class mqttServ : public PubSubClient, public WiFiClient
{
public:
    void init();
    void loop();
    void pub( char *topic, char *payload );
    void sub( char *topic );

    WiFiClient espClient;
    PubSubClient client(espClient);

private:
    void setupWifi();
    void reconnect();
    void callback( char *topic, byte *payload, unsigned int length );

    char *_ssid;
    char *_password;
    char *_mqtt_server;
    int   _port;


};


#endif