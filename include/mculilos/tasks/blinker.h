#pragma once

class Blinker
{
private:
    int _pin;
    volatile int _freq;
    unsigned long _lastChange;
    bool _state;
    volatile byte _mode; // 0 = off, 1 = on, 2 = blinking

public:
    Blinker(int pin, int freq, byte mode);
    void setBlinking(int freq);
    void setOn(bool on);
    static int begin(Blinker *blinker);
    static int cycle(Blinker *blinker, BusMsg *msgPtr);
    static int end(Blinker *blinker);
    static s_task_lifecycles lifecycles;
    static unsigned int type;
 };