#include "los.h"

TaskID CreatTask(void (*task_init)(void), void (*task)(void), BaseType interval)
{
    uint8_t i;
    for(i=0; i<config_MAX_TASK; i++){
        if(NULL == Los.TcbList[i].task){
            Los.TcbList[i].task = task;
            Los.TcbList[i].task_init = task_init;
            Los.TcbList[i].interval_time = interval;
            Los.TcbList[i].run_time = Los.current_time;
            Los.TcbList[i].task_init();
            return i;
        }
    }
    return 0xFF;
}

void Task_Scheduler(void)
{
    uint8_t i;
    for(i=0; i<config_MAX_TASK; i++){
        if((Los.current_time - Los.TcbList[i].run_time) > Los.TcbList[i].run_time){
            Los.TcbList[i].task();
            Los.TcbList[i].run_time = Los.current_time;
        }
    }
}

void Task_Sys_Clk(void)
{
    uint8_t i;
    Los.current_time++;
    for(i=0; i<config_MAX_TASK; i++){
        if(Los.TcbList[i].task){
            Los.TcbList[i].run_time = Los.current_time;
        }
    }
}