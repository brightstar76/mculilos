#pragma once

#include <ESP8266WiFi.h>

class WiFiDevice {
    private:
        char _essid[32];
        char _password[32];
    public:
        WiFiDevice();
        ESP8266WiFiClass *getWiFi(void);
        static unsigned int deviceType; // = 0xf1;

};