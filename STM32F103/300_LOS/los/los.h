/* 
Version: LOS V0.0.1 
Author:  Labc
Time:    2018/01/10
Add:    
    1. add base os.
*/


#ifndef __LOS_H__
#define __LOS_H__

#include "los_arch.h"
#include "los_task.h"
#include "los_mem.h"
#include "os_printf.h"

typedef struct{
    TaskTCB TcbList[config_MAX_TASK];
    uint32_t current_time;
}LOS;

extern LOS Los;

void Os_Init(void);

#endif /* __LOS_H__ */