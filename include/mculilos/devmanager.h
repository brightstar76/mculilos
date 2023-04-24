#pragma once

struct device_entry_t {
    unsigned int deviceType;
    void *devicePtr;
};

class DeviceManager {
    private:
    int _numDevices;
    device_entry_t *_deviceEntries;
    public:
    DeviceManager(device_entry_t *devEntries);
    void *findDeviceOfType(unsigned int type, byte index);
};


#ifdef ENABLE_HUMAN_READABLE_NAMES
struct device_type_with_name_t {
    byte deviceType;
    const char *name;
};
devicesTypeNames[] = {
    {
        0x00,
        "No such device"
    }
    {
        0x01,
        "4x4 Keypad"
    },
    {
        0x02,
        "Signal output"
    }
}
#endif