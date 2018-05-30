#ifndef __PID_H__
#define __PID_H__

#include "stdint.h"

//typedef struct {
//float SetSpeed;  
//    float ActualSpeed;  
//    float err;  
//    float err_last;  
//    float Kp,Ki,Kd;  
//    //λ��ʽpid  
//    float voltage;  
//    float integral;  
//    //����ʽpid  
//    float err_next;  
//    //�����ֱ���PID  
//    float umax;  
//    float umin;
//}PID_Struct;


extern int32_t PID_realize_R(int32_t dst_v, int32_t cur_v);   //����ʽPID;
extern int32_t PID_realize_L(int32_t dst_v, int32_t cur_v);   //����ʽPID;

#endif  /* __PID_H__ */