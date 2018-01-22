#ifndef __LOS_TASK_H__
#define __LOS_TASK_H__

#include "los_arch.h"

typedef void (*taskFunction)(void);

typedef struct{
    taskFunction task;
    taskFunction task_init;
    uint32_t run_time;
    uint32_t interval_time;
}TaskTCB;

TaskID CreatTask(taskFunction task_init, taskFunction task, BaseType interval);
void Task_Init(void);
void Task_Scheduler(void);
void Task_Sys_Clk(void);


#endif  /* __LOS_TASK_H__ */