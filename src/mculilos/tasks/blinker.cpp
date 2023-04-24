#include "mculilos/system.h"
#include "mculilos/tasks/blinker.h"

Blinker::Blinker(int pin, int freq, byte mode)
{
    _pin = pin;
    _freq = freq;
    _mode = mode;
    
}

void Blinker::setBlinking(int freq)
{
    _mode = 2;
    _freq = freq;
}

void Blinker::setOn(bool on)
{
    _mode = on ? 1 : 0;
}

int Blinker::begin(Blinker *blinker)
{
#define ctx blinker
    pinMode(ctx->_pin, OUTPUT);
    ctx->_mode = 0;
    ctx->_state = false;
    ctx->_lastChange = millis();
    digitalWrite(ctx->_pin, ctx->_state ? HIGH : LOW);
#undef ctx
    return 0;
}

s_task_lifecycles Blinker::lifecycles = {
        (int (*)(void *))Blinker::begin,
        (int (*)(void *, BusMsg *))Blinker::cycle,
        (int (*)(void *))Blinker::end};
//s_task_lifecycles Blinker::lifecycles = {NULL, NULL, NULL};

unsigned int Blinker::type = 0x08;

int Blinker::cycle(Blinker *blinker, BusMsg *msgPtr)
{
    #define ctx blinker

    if (blinker->_mode == 0)
        digitalWrite(blinker->_pin, LOW);
    else if (blinker->_mode == 1)
        digitalWrite(blinker->_pin, HIGH);
    else
    {
        if (millis() - ctx->_lastChange > (1000 / ctx->_freq))
        {
            ctx->_state = !ctx->_state;
            digitalWrite(ctx->_pin, ctx->_state ? HIGH : LOW);
            ctx->_lastChange = millis();
        }
    }
#undef ctx
    return 0;
}

int Blinker::end(Blinker *blinker)
{
    return 0;
}
