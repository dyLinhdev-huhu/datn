#include <Arduino.h>
#include "app.h"

unsigned long starttimeTemp;
unsigned long sampletimeTemp_ms = 1000;
extern unsigned long logTime;

extern int startFlag;

/// @brief 
app app;

void setup() {
  logTime = millis();

  app.initHardware();
  app.initClient();

  app.starttime = millis();
  starttimeTemp = millis();
}

void loop() {

  app.clientLoop();
  if(startFlag)
  {
    app.readSensor();
    app.uploadData();
    // if ((millis()-starttimeTemp) > sampletimeTemp_ms)
    // {
    //   Serial.println(".....");
    //   starttimeTemp = millis();
    // }
  }
}