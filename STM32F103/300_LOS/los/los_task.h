#ifndef __LOS_TASK_H__
#define __LOS_TASK_H__

#include "los.h"

typedef struct{
    BaseType *stack_top;
    BaseType *stack_task;
    uint32_t id;
    uint8_t priority;
    
}TaskTCB;


#endif  /* __LOS_TASK_H__ */