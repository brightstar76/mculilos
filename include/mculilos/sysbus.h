#pragma once

#include "mculilos/system.h"

#include <queue>



#define BUSMSG_BROADCAST (0x01)
#define BUSMSG_BROADCAST_ALL_TYPES (0x01 << 1)

class BusMsg
{
public:
    unsigned int _recipientType; // if non-zero, recipientIndex is of the index of the task OF GIVEN TYPE
    int _recipientIndex;
    int _senderIndex;  //
    unsigned int _senderType;
    int _senderIndexAbsolute; // absolute index of the sender, not in relation to type
    union
    {
        byte _command;
        byte _response;
    };
    byte _flags;
    void *_param;
    void *_chunk; // chuck is dynamically allocated and will be freed automatically by the SystemBus

    BusMsg(unsigned int toType, int toIndex, byte command, byte flags, void *param=NULL, void *chunk=NULL);
    void free(void);
};

class SystemBus
{
private:
    std::queue<BusMsg> _messages;

public:
    BusMsg &peekMessage(void);
    void removeMessage(void);
    bool available(void);
    int sendMessage(BusMsg msg);
};