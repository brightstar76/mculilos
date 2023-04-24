#include "mculilos/system.h"
#include "mculilos/sysbus.h"

#include "mculilos/devices/keypads/keyb4x4.h"
#include "mculilos/tasks/admin.h"
#include "mculilos/tasks/blinker.h"


TaskScheduler::TaskScheduler()
{
    _numTasks = 0;
}

TaskScheduler::TaskScheduler(s_task_t *startupTasks)
{
    _startupTasks = startupTasks;
    for (s_task_t * t = _startupTasks; true; t++)
    {
        if (t->lifecycles == NULL && t->contextPtr == NULL)
            break;
        else _numTasks++;
    }
}

int TaskScheduler::begin()
{
    int numFailed = 0;
    for (int i = 0; i < _numTasks; i++)
    {
        if (_startupTasks[i].lifecycles->begin(_startupTasks[i].contextPtr) != 0)
            numFailed++;
    }

    return -numFailed;
}

int TaskScheduler::end()
{
    int numFailed = 0;
    for (int i = 0; i < _numTasks; i++)
    {
        if (_startupTasks[i].lifecycles->end(_startupTasks[i].contextPtr) != 0)
            numFailed++;
    }

    return -numFailed;
}

int TaskScheduler::cycle()
{
    BusMsg *msgPtr = NULL;

    if (gLilOS.bus.available())
    {
        msgPtr = &gLilOS.bus.peekMessage();
    }


    for (int i = 0; i < _numTasks; i++)
    {
        unsigned long now = millis();
        bool msgSuitable = msgPtr && (msgPtr->_recipientType == _startupTasks[i].type || msgPtr->_flags & BUSMSG_BROADCAST_ALL_TYPES);
        if (_startupTasks[i].lifecycles->cycle(_startupTasks[i].contextPtr, msgSuitable?msgPtr:NULL) != 0)
        {
            Serial.print("Restarting task: ");
            Serial.println(i);
            _startupTasks[i].lifecycles->end(_startupTasks[i].contextPtr);
            _startupTasks[i].lifecycles->begin(_startupTasks[i].contextPtr);
        }
        if (now + 100 < millis())
        {
            Serial.print("PROFILER: Task ");
            Serial.print(i);
            Serial.print(" took ");
            Serial.print(millis() - now);
            Serial.println("ms for its cycle!");
        }
            
    }

    if (msgPtr)
        gLilOS.bus.removeMessage();
    return 0;
}

void *TaskScheduler::getTaskContextPtr(int index)
{
    if (_numTasks >= index)
        return NULL;
    return _startupTasks[index].contextPtr;
}

void *TaskScheduler::getTaskContextPtrByType(unsigned int type, int index)
{
    for (int taskNo = 0; taskNo < _numTasks; taskNo++)
    {
        s_task_t *ptr = &_startupTasks[taskNo];
        if (ptr->type == type)
        {
            if (index == 0)
                return ptr->contextPtr;
            else index--;
        }
    }
    return NULL;
}

/*int TaskScheduler::registerTask(int (*begin)(void*), int (*cycle)(void*), int (*end)(void*), void *contextPtr)
{
    _tasks[_numTasks].begin = begin;
    _tasks[_numTasks].cycle = cycle;
    _tasks[_numTasks].end = end;
    _tasks[_numTasks].contextPtr = contextPtr;
    _numTasks++;
}*/