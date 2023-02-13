#ifndef _APP_H_
#define _APP_H_

#include <Arduino.h>

#define SENSOR_PIN      12
#define LED_PIN         16
#define SEALEVELPRESSURE_HPA (1013.25)

class app{
public:
    app();
    void initHardware();
    void initClient();
    void readSensor();
    void uploadData();
    void clientLoop();
    void reConnect();
    void setStartTime(unsigned long stime);

    unsigned long duration;
    unsigned long starttime;
    unsigned long lowpulseoccupancy;
    float ratio;
    float concentration;
// private:
//     void onDataMqtt(char* topic, byte* payload, unsigned int length);
};

#endif //_APP_H_