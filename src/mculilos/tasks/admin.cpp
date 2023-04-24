#include "mculilos/system.h"

#include "mculilos/devices/keypads/keyb4x4.h"
#include "mculilos/tasks/admin.h"

extern TaskScheduler sched;

const char *keyNames[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "9", "0", "#", "ENTER", "CANCEL", "UP", "DOWN"};

Admin::Admin(Keyb4x4 *keybPtr)
{
    _keybPtr = keybPtr;
    _inputCommand[0] = '\0';
    _state = 0;
}

unsigned int Admin::type = 0x37;

int Admin::cycleInternal(BusMsg *msgPtr)
{
    // Serial.print("DEBUG _state: ");
    // Serial.println(_state);

    bool anyPressed = false;
    byte k;

    switch (_state)
    {
    case 0x00:
        Serial.println("Admin access granted, awaiting commands.");
        _state = 0x30;
        break;

    case 0x01:
        for (int i = 0; i < 16; i++)
        {
            if (_keybPtr->keyPressed(i))
            {
                Serial.print("INPUT: ");
                Serial.println(keyNames[i]);
                _state = 0x02;
                // break;
            }
        }
        break;

    case 0x02:

        for (int i = 0; i < 16; i++)
        {
            if (_keybPtr->keyPressed(i))
            {
                anyPressed = true;
                break;
            }
        }

        if (!anyPressed)
        {
            _state = 0x01;
        }
        break;

    case 0x30:
        Serial.print("> ");
        Serial.flush();
        _state = 0x31;
        break;

    case 0x31:
        k = _keybPtr->readOneKey();
        if (k == 0xff)
            break;
        if (k < 10)
        {
            if (strlen(_inputCommand) < ADMIN_MAX_COMMAND_LENGTH)
            {
                strcat(_inputCommand, keyNames[k]);
                Serial.print(keyNames[k]);
            }

            _state = 0x32;
            break;
        }
        else if (!strcmp(keyNames[k], "ENTER"))
        {
            Serial.println("");
            int result = executeCommand();
            if (result > 0)
                _state = result;
            else _state = 0x33;
        }

        break;
    case 0x32:
        if (_keybPtr->nothingPressed())
            _state = 0x31;
        break;

    case 0x33:
        if (_keybPtr->nothingPressed())
            _state = 0x30;
        break;
    
    case 0x66:
        if (msgPtr && msgPtr->_command == 0x01)
        {
            if ((int)msgPtr->_param == 0)
                Serial.println("Failed to get Wi-Fi back online.");
            else if ((int)msgPtr->_param == 1)
                Serial.println("Wi-Fi connection reset succesfully!");
            _state = 0x32;
        }
        break;

    default:
        Serial.println("Internal error (uknown comms state)");
    }

    // Serial.print("DEBUG done processing _state: ");
    // Serial.println(_state);

    return 0;
}

#ifndef blinker_ctx_t
struct blinker_ctx_t
{
    int pin;
    int freq;
    unsigned long lastChange;
    bool state;
    byte mode;
};
#endif

int Admin::executeCommand(void)
{
    int inputNumber = atoi(_inputCommand);
    _inputCommand[0] = '\0';
    --inputNumber;
    //Serial.println(inputNumber);
    if (inputNumber == 4) // Five, actually
    {
        gLilOS.bus.sendMessage(BusMsg(0xf1, 0, 0x03, 0));
        Serial.println("Sent reset command to Wi-Fi manager...");
        return 0x66;
    }
    if (inputNumber < 0)
    {
        Serial.println("Invalid number");
        return -1;
    }
    bool dirUp = inputNumber%2;
    blinker_ctx_t *ourBlinker = (blinker_ctx_t *)sched.getTaskContextPtrByType(0x08, inputNumber / 2);
    if (ourBlinker == NULL)
    {
        Serial.println("Blinker not detected");
        return -1;
    } else {
        ourBlinker->freq = ourBlinker->freq * (dirUp ? 2.0 : 0.5);
        Serial.println("Blinker frequency seuccesfully changed!");
        return 0;
    }



    return 0;

    blinker_ctx_t *blinker = (blinker_ctx_t *)sched.getTaskContextPtr(1);  // built-in LED blinker
    blinker_ctx_t *blinker2 = (blinker_ctx_t *)sched.getTaskContextPtr(2); // additional blinker
    
    if (inputNumber == 1)
    {
        blinker->freq /= 2;
    }
    if (inputNumber == 2)
    {
        blinker->freq *= 2;
    }
    if (inputNumber == 3)
    {
        blinker2->freq /= 2;
    }
    if (inputNumber == 4)
    {
        blinker2->freq *= 2;
    }
    Serial.print("Unreckognized command: ");
    Serial.println(_inputCommand);
    _inputCommand[0] = '\0';
    return -1;
}


int Admin::begin(Admin *adm)
{
  Serial.begin(9600);

  return 0;
}

int Admin::cycle(Admin *adm, BusMsg *msgPtr)
{
  adm->cycleInternal(msgPtr);
  return 0;
}

int Admin::end(Admin *adm)
{
  Serial.end();
  return 0;
}

s_task_lifecycles Admin::lifecycles = {
        (int (*)(void *))Admin::begin,
        (int (*)(void *, BusMsg *))Admin::cycle,
        (int (*)(void *))Admin::end};