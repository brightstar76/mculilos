#pragma once

#include <EEPROM.h>
#include <vector>

class EEPROMReservation {
    public:
        size_t _size;
        unsigned int _offset;
        EEPROMReservation(size_t size, unsigned int offset);
};

class EEPROMDevice {
    private:
        uint8_t *_mirror;
        size_t _memSize;
        std::vector<EEPROMReservation> _reservations;
        bool _isInitialized;
    public:
        EEPROMDevice();
        void initialize(void);
        void loadMemory(void);
        //int writeChunk(void *chunk, size_t chunkSize, unsigned int address = 0);
        //void *accessChunk(unsigned int address = 0);
        void *accessReservedChunk(int chunkNo = 0);
        bool writeReservedChunk(int chunkNo, void *chunkData);
        int reserveChunk(size_t size);
        static unsigned int deviceType; // = 0xee;
};
