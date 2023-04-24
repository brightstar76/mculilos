#include "mculilos/system.h"
#include "mculilos/tasks/mqtt.h"
#include "mculilos/devices/storage/eeprom.h"
#include "mculilos/tasks/blinker.h"
#include "mculilos/devices/sensors/humidity.h"

MQTTTask::MQTTTask()
{
    _clientPtr = NULL;
    _state = 0;
    _numSwitches = 0;
}

MQTTTask *currentSelf = NULL;

void MQTTCallback(char *topic, byte *payload, unsigned int length)
{
    Serial.print("Message on topic: ");
    Serial.print(topic);
    Serial.print(", len = ");
    Serial.println(length);

    const char *prefix = "onoff-switch-set-";
    const char *prefix2 = "onoff-switch-get-";
    const char *prefix3 = "onoff-switch-check-";

    if (0 == memcmp(topic, prefix3, strlen(prefix3)))
    {
        Serial.println("Ping?");
        char numArg[16];
        strncpy(numArg, topic + strlen(prefix3), 15);
        int num = atoi(numArg);
        num--;
        if (currentSelf->_numSwitches > num && num >= 0)
        {
            char topicStr[80];
            snprintf(topicStr, 79, "onoff-switch-heartbeat-%d", num+1);
            currentSelf->_clientPtr->publish(topicStr, "online");
            Serial.println("Pong!");
        }
    }
    else if (0 == memcmp(topic, prefix, strlen(prefix)))
    {
        Serial.println("Reacting.");
        char numArg[16];
        strncpy(numArg, topic + strlen(prefix), 15);
        int num = atoi(numArg);
        if (num == -1)
        {
            Serial.println("Malformed number.");
            return;
        }
        else
        {
            num--;  // indexing starting from zero
            if (num >= currentSelf->_numSwitches || num < 0)
            {
                Serial.println("Switch number out of range!");
                return;
            } else 
            {
                int newState = -1;
                if (length == 2 && memcmp(payload, "on", 2) == 0)
                    newState = 1;
                if (length == 3 && memcmp(payload, "off", 3) == 0)
                    newState = 0;

                if (newState == -1)
                {
                    Serial.println("Uncomprehensible command (neither on not off)...");
                    return;
                }
                
                Serial.print(newState ? "Enabling: " : "Disabling: ");
                Serial.println(num);
                gLilOS.bus.sendMessage(BusMsg(0x0f, 0, newState ? 0x10 : 0x11, 0, (void *) num));
            }
        }
    }
    else if (0 == memcmp(topic, prefix2, strlen(prefix2)))
    {
        Serial.println("Reacting.");
        char numArg[16];
        strncpy(numArg, topic + strlen(prefix), 15);
        int num = atoi(numArg);
        if (num == -1)
        {
            Serial.println("Malformed number.");
            return;
        }
        else
        {
            num--;  // indexing starting from zero
            if (num >= currentSelf->_numSwitches || num < 0)
            {
                Serial.println("Switch number out of range!");
                return;
            } else 
            {
                            
                Serial.print("Fetching: ");
                Serial.println(num);
                if (currentSelf->_state == 0x32)
                {
                    gLilOS.bus.sendMessage(BusMsg(0x0f, 0, 0x12, 0, (void *) num));
                    currentSelf->_state = 0x60;
                }
                //gLilOS.bus.sendMessage(BusMsg(0x0f, 0, newState ? 0x10 : 0x11, 0, (void *) num));
            }
        }
    }
}

int MQTTTask::begin(MQTTTask *self)
{
    EEPROMDevice *ee = (EEPROMDevice *)gLilOS._devManPtr->findDeviceOfType(0xee, 0);
    self->_eepromChunkNo = ee->reserveChunk(sizeof(mqtt_creds_t));

    /* // moved to cycle
    self->_clientPtr = new PubSubClient(self->_wifiClient);
    self->_clientPtr->setServer("192.168.100.49", 1883);
    currentSelf = self;
    self->_clientPtr->setCallback(MQTTCallback);
    self->_state = 0;*/
    self->_state = 0x00;
    return 0;
}

int MQTTTask::cycle(MQTTTask *self, BusMsg *msgPtr)
{
    EEPROMDevice *ee = (EEPROMDevice *)gLilOS._devManPtr->findDeviceOfType(0xee, 0);
    Blinker *blinker = (Blinker *)gLilOS._schedPtr->getTaskContextPtrByType(0x08, 0);
    mqtt_creds_t *credsPtr;

    if (msgPtr && msgPtr->_command == 0xf1)
    {
        if ((int)msgPtr->_param == 1)
        {
            // net up, react
            self->_state = 0xb0;
        }
        else if ((int)msgPtr->_param == 0)
        {
            // net down, react
            self->_state = 0x00;
        }
    }

    switch (self->_state)
    {
    case 0:
        // #define PROGRAMMING_MQTT_EEPROM

        #ifdef PROGRAMMING_MQTT_EEPROM
        mqtt_creds_t newCreds;
        memset(&newCreds, 0, sizeof(mqtt_creds_t));
        strcpy(newCreds.hostname, "192.168.100.49");
        strcpy(newCreds.clientId, "ClientId-7777");
        strcpy(newCreds.username, "");
        strcpy(newCreds.password, "");
        newCreds.port = 1883;
        ee->writeReservedChunk(self->_eepromChunkNo, (void *) &newCreds);
        #endif
        self->_state = 1;
        break;

    case 1:
        break;

    case 0x2e: // we jump here for a delay before we attempt to connect to MQTT broker again. Only way so far to avoid blocking the OS loop entirely...
        blinker->setBlinking(3);
        self->_lastFailedAttemptMillis = millis();
        self->_state = 0x2f;
        break;
    case 0x2f:
        if (self->_lastFailedAttemptMillis + 10 * 1000 < millis())
            self->_state = 0x30;
        break;
    case 0x30: // jumped to when net is up
        currentSelf = self;
        Serial.println("Connecting to MQTT...");
        self->_blockingTimeMeasurement = millis();
        blinker->setOn(false);
        if (self->_clientPtr)
            delete self->_clientPtr;

        

        credsPtr = (mqtt_creds_t *) ee->accessReservedChunk(self->_eepromChunkNo);

        self->_clientPtr = new PubSubClient(self->_wifiClient);
        Serial.print("Hostname: "); Serial.println(credsPtr->hostname); // debug
        Serial.print("Port: "); Serial.println(credsPtr->port); // debug
        Serial.print("Client ID: "); Serial.println(credsPtr->clientId); // debug
        self->_clientPtr->setServer(credsPtr->hostname, credsPtr->port/*1883*/);
        //self->_clientPtr->setServer("localhost", 1883);
        currentSelf = self;
        self->_clientPtr->setCallback(MQTTCallback);
        self->_wifiClient.setTimeout(2500);
        if (!strlen(credsPtr->username) || !strlen(credsPtr->password))
            self->_clientPtr->connect(credsPtr->clientId);
        else self->_clientPtr->connect(credsPtr->clientId, credsPtr->username, credsPtr->password);
        //self->_clientPtr->connect("ClientId7777");
        self->_state = 0x031;
        break;

    case 0x31:
        currentSelf = self;
        if (self->_clientPtr->connected())
        {
            Serial.println("Connected to MQTT!");
            blinker->setOn(true);
            char topicStr[80];
            for (int i = 0; i < self->_numSwitches; i++)
            {
                snprintf(topicStr, 79, "onoff-switch-set-%d", i+1);
                self->_clientPtr->subscribe(topicStr);
                snprintf(topicStr, 79, "onoff-switch-get-%d", i+1);
                self->_clientPtr->subscribe(topicStr);
                snprintf(topicStr, 79, "onoff-switch-check-%d", i+1);
                self->_clientPtr->subscribe(topicStr);
            }
            self->_lastHeartbeat = 0;
            self->_state = 0x32;
        }
        else
        {
            Serial.print("Connection to MQTT broker refused, blocking time = ");
            Serial.println(millis() - self->_blockingTimeMeasurement);
            self->_state = 0x2e;
        }
        break;

    case 0x32:
        currentSelf = self;
        if (self->_clientPtr->connected())
        {
            if (millis() > self->_lastHeartbeat+1000*6)
            {
                for (int i = 0; i < self->_numSwitches; i++)
                {
                    char topicStr[80];
                    snprintf(topicStr, 79, "onoff-switch-heartbeat-%d", i+1);
                    self->_clientPtr->publish(topicStr, "online");
                    self->_lastHeartbeat = millis();
                }
                HumidityAndTemperatureSensor *sensor = (HumidityAndTemperatureSensor *) gLilOS._devManPtr->findDeviceOfType(0x07, 0);
                float temperature = 0.0, humidity = 0.0;
                sensor->measure(&humidity, &temperature);
                char buf[32];

                snprintf(buf, 31, "%f", humidity);
                self->_clientPtr->publish("sensor-humidity-1", buf);

                snprintf(buf, 31, "%f", temperature);
                self->_clientPtr->publish("sensor-temperature-1", buf);
                
            }
            self->_clientPtr->loop();
        }
        else
            self->_state = 0x30;
        break;

    case 0x60:
        if (msgPtr && (msgPtr->_command == 0x13 || msgPtr->_command == 0x14))
        {
            bool state = msgPtr->_command == 0x13;
            char topicStr[80];
            snprintf(topicStr, 79, "onoff-switch-status-%d", ((int) msgPtr->_param)+1);
            self->_clientPtr->publish(topicStr, state ? "on" : "off");
            self->_state = 0x32;
        }
        break;

    case 0xb0:
        Serial.println("Querying ON/OFF service...");
        gLilOS.bus.sendMessage(BusMsg(0x0f, 0, 0x15, 0));
        self->_state = 0xb1;
        break;

    case 0xb1:
        if (msgPtr && msgPtr->_command == 0x15)
        {
            self->_numSwitches = (int) msgPtr->_param;
            self->_state = 0x30;
            Serial.println("Query successful");
        }
        break;
    
    }

    
    return 0;
}

int MQTTTask::end(MQTTTask *self)
{
    delete self->_clientPtr;
    return 0;
}

unsigned int MQTTTask::type = 0x99;
s_task_lifecycles MQTTTask::lifecycles = {
    (int (*)(void *))MQTTTask::begin,
    (int (*)(void *, BusMsg *))MQTTTask::cycle,
    (int (*)(void *))MQTTTask::end};