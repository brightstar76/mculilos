#include "mculilos/system.h"

#include "mculilos/devices/network/wifi.h"




WiFiDevice::WiFiDevice(/*const char *essid, const char *password*/)
{
    /*strncpy(_essid, essid, 31);
    strncpy(_password, password, 31);
    WiFi.begin(_essid, _password);*/
}

ESP8266WiFiClass *WiFiDevice::getWiFi(void)
{
    return &WiFi;
}

unsigned int WiFiDevice::deviceType = 0xf1;
