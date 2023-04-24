#pragma once

#define SCHEDULER_NUM_TASKS 4

struct s_task_lifecycles {
    int (*begin)(void*);
    int (*cycle)(void*, BusMsg *msgPtr);
    int (*end)(void*);
};

struct s_task_t {
    s_task_lifecycles *lifecycles;
    void *contextPtr;
    unsigned int type;
};

class TaskScheduler {
    private:
    s_task_t *_startupTasks;
    byte _numTasks;
    //s_task_t _tasks[SCHEDULER_NUM_TASKS]; 
    inline void test()
    {
        
    }
    public:
    //int registerTask(int (*begin)(void*), int (*cycle)(void)*, int (*end)(void*), void *contextPtr);
    TaskScheduler(s_task_t *startupTasks);
    TaskScheduler();
    int begin(void);
    int end(void);
    int cycle(void);
    void *getTaskContextPtr(int index);
    void *getTaskContextPtrByType(unsigned int type, int index);
};