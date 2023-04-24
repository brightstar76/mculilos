#include "mculilos/devices/storage/eeprom.h"



EEPROMDevice::EEPROMDevice()
{
    _mirror = NULL;
    _memSize = 0;
    _isInitialized = false;
}

EEPROMReservation::EEPROMReservation(size_t size, unsigned int offset)
{
    _size = size;
    _offset = offset;
}

void EEPROMDevice::initialize(void)
{
    _mirror = NULL;


    EEPROM.begin(_memSize);
    loadMemory();

    _isInitialized = true;

}

void EEPROMDevice::loadMemory(void)
{
    _mirror = new uint8_t [_memSize];

    for (unsigned int i = 0; i < _memSize; i++)
    {
        _mirror[i] = EEPROM.read(i);
    }
}

/*
int EEPROMDevice::writeChunk(void *chunk, size_t size, unsigned int address)
{
    if (address + size > _memSize)
        return -1;

    memcpy(_mirror, chunk, size);

    for (unsigned int i = address; i < address + size; i++)
    {
        EEPROM.write(i, _mirror[i]);
    }

    return EEPROM.commit() ? 0 : -1;
}*/

/*
void *EEPROMDevice::accessChunk(unsigned int address)
{
    return &_mirror[address];
}*/

void *EEPROMDevice::accessReservedChunk(int chunkNo)
{
    if (!_isInitialized) // no need for any task to call initialize, but
        initialize();
    return &_mirror[_reservations[chunkNo]._offset];
}

bool EEPROMDevice::writeReservedChunk(int chunkNo, void *chunkData)
{
    if (!_isInitialized) // no need for any task to call initialize, but
        initialize();
        
    memcpy(_mirror + _reservations[chunkNo]._offset, chunkData, _reservations[chunkNo]._size);
    for (unsigned int i = _reservations[chunkNo]._offset; i < _reservations[chunkNo]._offset + _reservations[chunkNo]._size; i++)
    {
        EEPROM.write(i, _mirror[i]);
    }
    return EEPROM.commit();
}

int EEPROMDevice::reserveChunk(size_t size)
{
    if(_isInitialized)
        return -1;

    _reservations.push_back(EEPROMReservation(size, _memSize));
    _memSize += size;
    return _reservations.size() - 1;
}

unsigned int EEPROMDevice::deviceType = 0xee;