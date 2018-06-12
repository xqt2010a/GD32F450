#ifndef __PID_H__
#define __PID_H__

#include "stdint.h"

//typedef struct {
//float SetSpeed;  
//    float ActualSpeed;  
//    float err;  
//    float err_last;  
//    float Kp,Ki,Kd;  
//    //位置式pid  
//    float voltage;  
//    float integral;  
//    //增量式pid  
//    float err_next;  
//    //抗积分饱和PID  
//    float umax;  
//    float umin;
//}PID_Struct;

#define PID_BUF_LEN     37

extern uint8_t pid_buf[PID_BUF_LEN];

extern int32_t PID_realize_R(int32_t dst_v, int32_t cur_v, uint32_t count);   //增量式PID;
extern int32_t PID_realize_L(int32_t dst_v, int32_t cur_v, uint32_t count);   //增量式PID;
extern int32_t PID_Correct_Len(uint32_t dst, uint32_t cur);   //位置式PID

#endif  /* __PID_H__ */