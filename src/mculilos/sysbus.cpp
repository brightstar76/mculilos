#include "mculilos/system.h"
#include "mculilos/sysbus.h"


BusMsg::BusMsg(unsigned int toType, int toIndex, byte command, byte flags=0, void *param, void *chunk)
{
    _recipientType = toType;
    _recipientIndex = toIndex;
    _command = command;
    _param = param;
    _chunk = chunk;
    _flags = flags;
}

void BusMsg::free(void)
{
    if (_chunk != NULL)
        ::free(_chunk);
}

int SystemBus::sendMessage(BusMsg msg)
{
    _messages.push(msg);
    return 0;
}

BusMsg &SystemBus::peekMessage(void)
{
    BusMsg &ret = _messages.front();
    return ret;
}

void SystemBus::removeMessage(void)
{
    BusMsg &msg = _messages.front();
    msg.free();
    _messages.pop();
}

bool SystemBus::available(void)
{
    return _messages.size() > 0;
}