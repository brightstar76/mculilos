#include "mculilos/devices/sensors/humidity.h"

HumidityAndTemperatureSensor::HumidityAndTemperatureSensor(int outPin, int dhtType)
{
    _dht = new DHT_Unified(outPin, dhtType);
    _dht->begin();
}

int HumidityAndTemperatureSensor::measure(float *humidity, float *temperature)
{
    sensors_event_t event;
    _dht->temperature().getEvent(&event);
    (*temperature) = (signed char)event.temperature;
    _dht->humidity().getEvent(&event);
    (*humidity) = event.relative_humidity;
    return 0;
}

unsigned int HumidityAndTemperatureSensor::deviceType = 0x07;