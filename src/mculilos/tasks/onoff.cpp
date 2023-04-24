#include "mculilos/system.h"
#include "mculilos/tasks/onoff.h"

OnOffSwitch::OnOffSwitch(int pin, bool initialState, bool invert)
{
    _pin = pin;
    _state = initialState;
    _invertOutput = invert;
}

OnOff::OnOff(int numberOfOutputs, int *pins, bool *initialStates, bool *inverts)
{
    for (int i = 0; i < numberOfOutputs; i++)
    {
        _switches.push_back(OnOffSwitch(pins[i], initialStates[i], inverts[i]));
    }
}

int OnOff::begin(OnOff *self)
{
    for (int i = 0; i < self->_switches.size(); i++)
    {
        pinMode(self->_switches[i]._pin, OUTPUT);
        bool state =  self->_switches[i]._state;
        bool inversion = self->_switches[i]._invertOutput;
        digitalWrite(self->_switches[i]._pin, (inversion?!state:state) ? HIGH : LOW);
    }
    self->_state = 0;
    return 0;
}

int OnOff::cycle(OnOff *self, BusMsg *msgPtr)
{
    if (msgPtr && (msgPtr->_command == 0x10 || msgPtr->_command == 0x11)) // 0x10 for 'set state to on', 0x11 for 'set state to off'
    {
        bool newState = msgPtr->_command == 0x10 ? true : false;
        int pinNo = self->_switches[((int) msgPtr->_param)%self->_switches.size()]._pin;
        bool inversion = (self->_switches[((int) msgPtr->_param)%self->_switches.size()]._invertOutput ? true : false);

        digitalWrite(pinNo, (inversion?!newState:newState) ? HIGH : LOW);
        self->_switches[((int) msgPtr->_param)%self->_switches.size()]._state = newState;
        gLilOS.bus.sendMessage(BusMsg(0x99, 0, msgPtr->_command, 0, msgPtr->_param)); // acknowledge state has been updated

    }
    if (msgPtr && msgPtr->_command == 0x12) // query state
    {
        int outputNo = ((int) msgPtr->_param) % self->_switches.size();
        if (self->_switches[outputNo]._state == true)
            gLilOS.bus.sendMessage(BusMsg(0x99, 0, 0x13, 0, msgPtr->_param));   // ox13 means the state is 'on'
        else gLilOS.bus.sendMessage(BusMsg(0x99, 0, 0x14, 0, msgPtr->_param));  // 0x14 means the state is 'off'
    }
    if (msgPtr && msgPtr->_command == 0x15) // query number of outputs
    {
        Serial.println("On/Off: Responding to query.");
        gLilOS.bus.sendMessage(BusMsg(0x99, 0, 0x15, 0, (void *) self->_switches.size()));
    }
    return 0;
}

int OnOff::end(OnOff *self)
{
    
    return 0;
}

unsigned int OnOff::type = 0x0f;
s_task_lifecycles OnOff::lifecycles = {
    (int (*)(void *))OnOff::begin,
    (int (*)(void *, BusMsg *))OnOff::cycle,
    (int (*)(void *))OnOff::end};
