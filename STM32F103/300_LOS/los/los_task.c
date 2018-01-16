#include "los_task.h"
#include "los_mem.h"

TaskTCB *TcbList[config_MAX_TASK] = {NULL};

BaseType xTask_Creat(osFunction task, uint16_t task_depth, uint8_t task_priority, void * task_parameter)
{
    uint8_t i, new_task_index;
    TaskTCB * new_tcb;
    
    BaseType *task_top = (BaseType *)os_Malloc(sizeof(BaseType)*task_depth);
    if(NULL == task_top){
        return false;
    }

    for(i=0; i<config_MAX_TASK; i++){
        if(NULL == TcbList[i]){
            new_task_index = i;
            break;
        }
    }
    if(config_MAX_TASK == new_task_index){  
        return false; /* TcbList is full */
    }
    /* malloc a TaskTCB */
    new_tcb = (TaskTCB *)os_Malloc(sizeof(TaskTCB));
    if(NULL == new_tcb){
        os_Free(task_top);
        return false;
    }
    TcbList[new_task_index] = new_tcb;
    new_tcb->stack_top = task_top;
    new_tcb->id = (uint32_t)new_tcb;
    if(task_priority > config_MAX_PRIORITY){
        task_priority = config_MAX_PRIORITY;
    }
    new_tcb->priority = task_priority;
    
    
    return true;
}