#pragma once

#include <vector>

class OnOffSwitch {
    public:
    int _pin;
    bool _state;
    bool _invertOutput;
    OnOffSwitch(int pin, bool initialState, bool invert);
};

class OnOff
{
private:
    std::vector<OnOffSwitch> _switches;
    int _state;

public:
    OnOff(int numberOfOutputs, int *pins, bool *initialStates, bool *outputInversions);
    static int begin(OnOff *self);
    static int cycle(OnOff *self, BusMsg *msgPtr);
    static int end(OnOff *self);
    static s_task_lifecycles lifecycles;
    static unsigned int type;
 };