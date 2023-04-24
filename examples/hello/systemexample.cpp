#include <mculilos/system.h>

#include <mculilos/devices/keypads/keyb4x4.h>
#include <mculilos/tasks/admin.h>
#include <mculilos/tasks/blinker.h>
#include <mculilos/devices/network/wifi.h>
#include <mculilos/devices/storage/eeprom.h>
#include <mculilos/tasks/wifiman.h>
#include <mculilos/tasks/mqtt.h>
#include <mculilos/tasks/onoff.h>

extern TaskScheduler sched;
extern DeviceManager devMan;


//Keyb4x4 keyb(2, 0, 4, 5, 15, 13, 12, 14);
WiFiDevice myWifi;
EEPROMDevice eepromDev;

device_entry_t myDevices [] = {
  /*{
    Keyb4x4::deviceType,
    (void *) &keyb
  },*/
  {
    WiFiDevice::deviceType,
    (void *) &myWifi
  },
  {
    EEPROMDevice::deviceType,
    (void *) &eepromDev
  },
  {
    0x00,
    NULL
  }
};

DeviceManager devMan(myDevices);


//Admin adminCtx(&keyb);
//Blinker blinker1Ctx(/*LED_BUILTIN*/9, 8);
Blinker blinker2Ctx(16, 16, 0);
WiFiManager wifiManCtx;
MQTTTask mqttTaskCtx;


int onoffPins [] = {15, 13, 12, 14};
bool onoffInitialValues [] = {false, false, false, false};
bool onoffInverts [] = {false, false, false, false};
OnOff onoffCtx(4, onoffPins, onoffInitialValues, onoffInverts);

s_task_t myTasks[] = {
  /*{
    &Admin::lifecycles,
    (void *)&adminCtx,
    Admin::type
  },*/
  /*{
    &Blinker::lifecycles,
    (void *)&blinker1Ctx
  },*/
  {
    &Blinker::lifecycles,
    (void *)&blinker2Ctx,
    Blinker::type
  },
  {
    &WiFiManager::lifecycles,
    (void *) &wifiManCtx,
    WiFiManager::type
  },
  {
    &MQTTTask::lifecycles,
    (void *) &mqttTaskCtx,
    MQTTTask::type
  },
  {
    &OnOff::lifecycles,
    (void *) &onoffCtx,
    OnOff::type
  },
  {
    NULL,
    NULL
  }
};

TaskScheduler sched(myTasks);

LilOS gLilOS(&devMan, &sched);

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  gLilOS.run();
}