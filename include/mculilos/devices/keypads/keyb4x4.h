#pragma once

#define KEY_PRESSED_VAL LOW
#define KEY_RELEASED_VAL HIGH

/// #define USE_KEYPAD_DELAY

class Keyb4x4 {
    private:
    int _rowPins[4];
    int _colPins[4];
    public:
    Keyb4x4(int r1Pin, int r2Pin, int r3Pin, int r4Pin, int c1Pin, int c2Pin, int c3Pin, int c4Pin);
    bool keyPressed(int keyNo);
    bool nothingPressed(void);
    byte readOneKey(void);
    static unsigned int deviceType;
};