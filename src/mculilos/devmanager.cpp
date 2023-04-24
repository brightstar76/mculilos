#include "mculilos/system.h"

DeviceManager::DeviceManager(device_entry_t *devs)
{
    _deviceEntries = devs;
    for (device_entry_t *ptr = devs; (ptr->deviceType != 0x00 && ptr->devicePtr != NULL); ptr++)
        _numDevices++;
}

void *DeviceManager::findDeviceOfType(unsigned int type, byte index)
{
    void *ret = NULL;

    for (int i = 0; i < _numDevices; i++)
    {
        if (_deviceEntries[i].deviceType == type)
        {
            if (index == 0)
                return _deviceEntries[i].devicePtr;
            else index--;
        }
    }

    return ret;
}