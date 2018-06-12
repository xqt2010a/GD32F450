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

#define PID_BUF_LEN     37

extern uint8_t pid_buf[PID_BUF_LEN];

extern int32_t PID_realize_R(int32_t dst_v, int32_t cur_v, uint32_t count);   //����ʽPID;
extern int32_t PID_realize_L(int32_t dst_v, int32_t cur_v, uint32_t count);   //����ʽPID;
extern int32_t PID_Correct_Len(uint32_t dst, uint32_t cur);   //λ��ʽPID

#endif  /* __PID_H__ */