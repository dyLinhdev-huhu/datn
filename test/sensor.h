#ifndef _SENSOR_H_
#define _SENSOR_H_

typedef struct sensor_result
{
    unsigned long lowpulseoccupancy;
    float ratio;
    float concentration;
}rsSensor_t;


class sensor{
public:
    sensor(int pin);
    ~sensor();
    void loop();

private:
    int _pin;
    unsigned long _duration;
    unsigned long _starttime;
    unsigned long _sampletime_ms;
    unsigned long _lowpulseoccupancy;
    float _ratio;
    float _concentration;
};

#endif