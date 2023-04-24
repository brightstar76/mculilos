#include "mculilos/system.h"
#include "mculilos/devices/keypads/keyb4x4.h"


Keyb4x4::Keyb4x4(int r1Pin, int r2Pin, int r3Pin, int r4Pin, int c1Pin, int c2Pin, int c3Pin, int c4Pin)
{
    _rowPins[0] = r1Pin;
    _rowPins[1] = r2Pin;
    _rowPins[2] = r3Pin;
    _rowPins[3] = r4Pin;
    _colPins[0] = c1Pin;
    _colPins[1] = c2Pin;
    _colPins[2] = c3Pin;
    _colPins[3] = c4Pin;

    for (int i = 0; i < 4; i++)
    {
        pinMode(_rowPins[i], OUTPUT);
        digitalWrite(_rowPins[i], LOW);
        pinMode(_rowPins[i], INPUT_PULLUP);

        pinMode(_colPins[i], OUTPUT);
        digitalWrite(_colPins[i], KEY_RELEASED_VAL);
    }

}

bool Keyb4x4::keyPressed(int keyNo)
{
    int r = keyNo / 4;
    int c = keyNo % 4;

    for (int ci = 0; ci < 4; ci++)
    {
        if (ci!=c)
        {
            pinMode(_colPins[ci], OUTPUT);
            digitalWrite(_colPins[ci], KEY_RELEASED_VAL);
        }
        pinMode(_colPins[ci], c==ci ? OUTPUT : INPUT);
        
        //if (ci != c)
        ///    continue;
        
    }

    digitalWrite(_colPins[c], KEY_PRESSED_VAL);
    #ifdef USE_KEYPAD_DELAY
    delay(1);
    #endif
    

    int s = digitalRead(_rowPins[r]);

    digitalWrite(_colPins[c], KEY_RELEASED_VAL);
    pinMode(_colPins[c], INPUT);
    #ifdef USE_KEYPAD_DELAY
    delay(1);
    #endif

    if (s == KEY_PRESSED_VAL)
        return true;
    else return false;

/*
    digitalWrite(c, HIGH);
    delay(3);
    int st = digitalRead(r);
    digitalWrite(c, LOW);
    delay(3);

    if (st == HIGH)
        return true;
    else return false;
    */
}

byte Keyb4x4::readOneKey(void)
{
    for (int i = 0; i < 16; i++)
    {
        if (keyPressed(i))
        {
            return i;
        }
    }
    return 0xff;
}

bool Keyb4x4::nothingPressed(void)
{
    bool somethingPressed = false;

    for (int i = 0; i < 16; i++)
    {
        if (keyPressed(i))
        {
            somethingPressed = true;
            break;
        }
    }
    return !somethingPressed;
}

unsigned int Keyb4x4::deviceType = 0x01;