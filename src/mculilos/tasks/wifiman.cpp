#include "mculilos/system.h"
#include "mculilos/devices/storage/eeprom.h"
#include "mculilos/tasks/wifiman.h"

WiFiManager::WiFiManager(void)
{
}

#ifndef blinker_ctx_t
struct blinker_ctx_t
{
    int pin;
    volatile int freq;
    unsigned long lastChange;
    bool state;
    volatile byte mode;
};
#endif

int WiFiManager::begin(WiFiManager *self)
{
    blinker_ctx_t *blinkerPtr = (blinker_ctx_t *)gLilOS._schedPtr->getTaskContextPtrByType(0x08, 0);
    blinkerPtr->mode = 0;

    WiFiDevice *wifiDevPtr = (WiFiDevice *)gLilOS._devManPtr->findDeviceOfType(0xf1, 0);

    EEPROMDevice *eepromDevPtr = (EEPROMDevice *)gLilOS._devManPtr->findDeviceOfType(0xee, 0);
    self->_eepromChunkNo = eepromDevPtr->reserveChunk(sizeof(wifi_creds_t));

    self->_state = 0xfe;
    return 0;
}

int WiFiManager::cycle(WiFiManager *self, BusMsg *msgPtr)
{
    WiFiDevice *wifiDevPtr = (WiFiDevice *)gLilOS._devManPtr->findDeviceOfType(0xf1, 0);
    ESP8266WiFiClass *w = wifiDevPtr->getWiFi();
    blinker_ctx_t *blinkerPtr = (blinker_ctx_t *)gLilOS._schedPtr->getTaskContextPtrByType(0x08, 0);

    EEPROMDevice *eepromDevPtr = (EEPROMDevice *)gLilOS._devManPtr->findDeviceOfType(0xee, 0);
    wifi_creds_t *credsPtr = NULL;

    switch (self->_state)
    {
    case 0:
        blinkerPtr->mode = 0;
        credsPtr = (wifi_creds_t *)eepromDevPtr->accessReservedChunk(self->_eepromChunkNo);
        w->begin(credsPtr->essid, credsPtr->password);
        w->enableSTA(true);
        self->_state = 1;
        break;
    case 1:
        blinkerPtr->mode = 2;
        blinkerPtr->freq = 8;
        if (w->status() == WL_CONNECTED)
        {
            // Serial.println("Connected... "); // debug
            self->_state = 10;
            gLilOS.bus.sendMessage(BusMsg(0x37, 0, 0x01, 0, (void *)1));
            gLilOS.bus.sendMessage(BusMsg(0x99, 0, 0xf1, BUSMSG_BROADCAST | BUSMSG_BROADCAST_ALL_TYPES, (void *)1));
            blinkerPtr->mode = 1;
        }
        break;

    case 10:

        if (w->status() != WL_CONNECTED)
        {
            gLilOS.bus.sendMessage(BusMsg(0x99, 0, 0xf1, BUSMSG_BROADCAST | BUSMSG_BROADCAST_ALL_TYPES, (void *)0));
            self->_state = 0;
            // Serial.println("Reconnecting... "); // debug
        }
        if (msgPtr && msgPtr->_command == 0x03)
        {
            gLilOS.bus.sendMessage(BusMsg(0x99, 0, 0xf1, BUSMSG_BROADCAST | BUSMSG_BROADCAST_ALL_TYPES, (void *)0));
            w->disconnect(true);
            self->_state = 0;
        }
        break;

    case 0xfe:
        // #define WIFI_CONFIGURING

        #ifdef WIFI_CONFIGURING
                wifi_creds_t initialCreds;
                memset(&initialCreds, 0, sizeof(wifi_creds_t));
                strcpy(initialCreds.essid, "<ESSID>");
                strcpy(initialCreds.password, "<PASSWORD>");
                eepromDevPtr->writeReservedChunk(self->_eepromChunkNo, &initialCreds);
        #endif
        self->_state = 0;
        break;
    }
    return 0;
}

int WiFiManager::end(WiFiManager *self)
{
    return 0;
}

s_task_lifecycles WiFiManager::lifecycles = {
    (int (*)(void *))WiFiManager::begin,
    (int (*)(void *, BusMsg *))WiFiManager::cycle,
    (int (*)(void *))WiFiManager::end};

unsigned int WiFiManager::type = 0x0f1;