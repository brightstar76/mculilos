#pragma once

#include <Arduino.h>

#include "sysbus.h"
#include "multitask.h"
#include "devmanager.h"


class LilOS
{
public:
    DeviceManager *_devManPtr;
    TaskScheduler *_schedPtr;
    SystemBus bus;

    LilOS(DeviceManager *devManPtr, TaskScheduler *schedPtr);
    void run(void);

};


extern LilOS gLilOS;

#define gDevMan (*(gLilOS._devManPtr))
#define gSched (*(gLilOS._schedPtr))
