#ifndef _SMART_CONFIG_H_
#define _SMART_CONFIG_H_

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <EEPROM.h>

void smartConfig_Init(void);
bool testWifi(void);
void launchWeb(void);
void setupAP(void);
void getUserWifi(void);
void createWebServer(void);

#endif //_SMART_CONFIG_H_