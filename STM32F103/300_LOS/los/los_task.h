#ifndef __LOS_TASK_H__
#define __LOS_TASK_H__

#include "los.h"

typedef struct{
    uint32_t run_flag;
    osFunction task;
    uint32_t run_time;
    uint32_t interval_time;
}TaskTCB;

typedef struct{
    TaskTCB TcbList[config_MAX_TASK];
    uint32_t current_time;
}LOS;


#endif  /* __LOS_TASK_H__ */