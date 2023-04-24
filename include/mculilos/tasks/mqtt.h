#pragma once

#include <PubSubClient.h>
#include "mculilos/devices/network/wifi.h"
#include "mculilos/tasks/wifiman.h"
#include "mculilos/devices/storage/eeprom.h"

struct mqtt_creds_t {
    char hostname[64];
    char clientId[48];
    char username[32];
    char password[32];
    uint16_t port;
};

class MQTTTask
{
private:
    WiFiClient _wifiClient;
    
    
    unsigned int _lastFailedAttemptMillis;
    unsigned int _blockingTimeMeasurement;
    int _eepromChunkNo;

    
public:
    PubSubClient *_clientPtr;
    unsigned long _lastHeartbeat;
    int _numSwitches;
    int _state;
    MQTTTask();
    static int begin(MQTTTask *self);
    static int cycle(MQTTTask *self, BusMsg *msgPtr);
    static int end(MQTTTask *self);
    static s_task_lifecycles lifecycles;
    static unsigned int type;
 };