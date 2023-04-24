#pragma once

#include "mculilos/devices/network/wifi.h"

struct wifi_creds_t {
    char essid[32];
    char password[32];
};

class WiFiManager
{
private:
    
int _state;
int _eepromChunkNo;
// wifi_creds_t _creds;

public:
    WiFiManager(void);
    static int begin(WiFiManager *blinker);
    static int cycle(WiFiManager *blinker, BusMsg *msgPtr);
    static int end(WiFiManager *blinker);
    static s_task_lifecycles lifecycles;
    static unsigned int type;
 };