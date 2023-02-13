#include <Arduino.h>
#include "sensor.h"

sensor::sensor(int pin)
{
    _pin = pin;
    pinMode(_pin,INPUT);
    unsigned long _lowpulseoccupancy = 0;
    float _ratio = 0;
    float _concentration = 0;
}

void sensor::loop()
{
    rsSensor_t result;
    _duration = pulseIn( _pin, LOW );
    _lowpulseoccupancy = _lowpulseoccupancy + _duration;

    if (( millis() - _starttime ) > _sampletime_ms )
    {
        _ratio = _lowpulseoccupancy/( _sampletime_ms*10.0 );  // Integer percentage 0=>100
        _concentration = 1.1*pow(_ratio,3)-3.8*pow(_ratio,2)+520*_ratio+0.62; // using spec sheet curve
        Serial.print(_lowpulseoccupancy);
        Serial.print(",");
        Serial.print(_ratio);
        Serial.print(",");
        Serial.println(_concentration);
        _lowpulseoccupancy = 0;
        _starttime = millis();
    }
}