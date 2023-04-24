#include "mculilos/system.h"
#include "mculilos/sysbus.h"

LilOS::LilOS(DeviceManager *devManPtr, TaskScheduler *schedPtr)
{
    _devManPtr = devManPtr;
    _schedPtr = schedPtr;
}

void LilOS::run(void)
{
    for (;;)
    {
        _schedPtr->begin();
        while (_schedPtr->cycle() == 0)
            yield();
        _schedPtr->end();
    }
}