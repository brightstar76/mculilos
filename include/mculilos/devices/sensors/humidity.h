#pragma once

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

class HumidityAndTemperatureSensor {
    private:
        DHT_Unified *_dht;
    public:
    HumidityAndTemperatureSensor(int outPin, int dhtType);
    int measure(float *humidity, float *temperature);
    static unsigned int deviceType;
};

