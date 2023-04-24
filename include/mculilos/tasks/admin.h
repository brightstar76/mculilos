#pragma once

#define ADMIN_MAX_COMMAND_LENGTH 15

class Admin {
    private:
    Keyb4x4 *_keybPtr;
    char _inputCommand[ADMIN_MAX_COMMAND_LENGTH+1];
    byte _state;
    int cycleInternal(BusMsg *msgPtr);
    public:
    int executeCommand(void);
    Admin(Keyb4x4 *keybPtr);
    static int begin(Admin *adm);
    static int cycle(Admin *adm, BusMsg *msgPtr);
    static int end(Admin *adm);
    static s_task_lifecycles lifecycles;
    static unsigned int type;
};