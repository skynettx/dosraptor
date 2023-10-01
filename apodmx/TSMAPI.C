#include <stdlib.h>
#include "task_man.h"
#include "tsmapi.h"

typedef struct {
    task *t;
    void (*callback)(void);
    int paused;
} task_t;

#define MAX_TASKS 8

task_t tasks[MAX_TASKS];

void TSM_Install(int rate) {
    memset(tasks, 0, sizeof(tasks));
}

void tsm_funch(task *t) {
    int i = (int)t->data;
    if (!tasks[i].paused)
        tasks[i].callback();
}

int TSM_NewService(void(*function)(void), int rate, int priority, int pause) {
    int i;
    for (i = 0; i < MAX_TASKS; i++)
    {
        if (tasks[i].t == NULL)
            break;
    }
    if (i == MAX_TASKS)
        return -1;
    tasks[i].callback = function;
    tasks[i].paused = pause;
    tasks[i].t = TS_ScheduleTask(tsm_funch, rate, priority, (void*)i);
    TS_Dispatch();
    return i;
}

void TSM_DelService(int id) {
    if (id >= 0)
    {
        TS_Terminate(tasks[id].t);
        tasks[id].t = NULL;
    }
}

void TSM_PauseService(int id) {
    if (id >= 0)
    {
        tasks[id].paused = 1;
    }
}

void TSM_ResumeService(int id) {
    if (id >= 0)
    {
        tasks[id].paused = 0;
    }
}

void TSM_Remove(void) {
    TS_Shutdown();
}

