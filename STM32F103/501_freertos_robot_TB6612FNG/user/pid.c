#include "pid.h"

#define PID_KP_R  10     //0.2 = 200/1000
#define PID_KI_R  200      
#define PID_KD_R  10

#define PID_KP_L  10     //0.2 = 200/1000
#define PID_KI_L  200      
#define PID_KD_L  10


int32_t PID_realize_R(int32_t dst_v, int32_t cur_v)     //增量式PID
{
    int32_t err, increment;//, calc;
    static int32_t err_last = 0, err_next = 0;
    
    err = dst_v - cur_v;
    increment = PID_KP_R*(err - err_next) + PID_KI_R*err + PID_KD_R*(err - 2*err_next + err_last);
    //calc = cur_v + increment/1000;
    err_last = err_next;
    err_next = err;
    //return calc;
    return increment;
}

int32_t PID_realize_L(int32_t dst_v, int32_t cur_v)     //增量式PID
{
    int32_t err, increment;//, calc;
    static int32_t err_last = 0, err_next = 0;
    
    err = dst_v - cur_v;
    increment = PID_KP_L*(err - err_next) + PID_KI_L*err + PID_KD_L*(err - 2*err_next + err_last);
    //calc = cur_v + increment/1000;
    err_last = err_next;
    err_next = err;
    //return calc;
    return increment;
}




//void PID_init()
//{
//    //printf("PID_init begin \n");
//    pid.SetSpeed = 0.0;  
//    pid.ActualSpeed = 0.0;  
//    pid.err = 0.0;  
//    pid.err_last = 0.0;  
//    pid.voltage = 0.0;  
//    pid.integral = 0.0;  
//    pid.Kp = 0.2;  
//    pid.Ki = 0.015;  
//    pid.Kd = 0.2;  
//    //printf("PID_init end \n");
//}

//float positional_PID_realize(float speed)       //位置式PID  
//{  
//    pid.SetSpeed = speed;  
//    pid.err = pid.SetSpeed - pid.ActualSpeed;  
//    pid.integral += pid.err;  
//    pid.voltage = pid.Kp*pid.err + pid.Ki*pid.integral + pid.Kd*(pid.err - pid.err_last);  
//    pid.err_last = pid.err;  
//    pid.ActualSpeed = pid.voltage*1.0;  
//    return pid.ActualSpeed;  
//}
//
//float incremental_PID_realize(float speed) {    //增量式PID
//    pid.err_next = 0.0;  
//  
//    pid.SetSpeed = speed;  
//    pid.err = pid.SetSpeed - pid.ActualSpeed;  
//    float incrementSpeed = pid.Kp*(pid.err - pid.err_next) + pid.Ki*pid.err + pid.Kd*(pid.err - 22 * pid.err_next + pid.err_last);  
//    pid.ActualSpeed += incrementSpeed;  
//    pid.err_last = pid.err_next;  
//    pid.err_next = pid.err;  
//    return pid.ActualSpeed;  
//}
//
//float  IntegralSeparatio_PID_realize(float speed) { //积分分离PID    
//    int index;  
//    pid.Kp = 0.2;  
//    pid.Ki = 0.04;  
//    pid.Kd = 0.2;  
//  
//    pid.SetSpeed = speed;  
//    pid.err = pid.SetSpeed - pid.ActualSpeed;  
//    if (abs(pid.err) > 200) {  
//        index = 0;  
//    }  
//    else {  
//        index = 1;  
//        pid.integral += pid.err;  
//    }  
//    pid.voltage = pid.Kp*pid.err + index*pid.Ki*pid.integral + pid.Kd*(pid.err - pid.err_last);  
//    pid.err_last = pid.err;  
//    pid.ActualSpeed = pid.voltage*1.0;  
//    return pid.ActualSpeed;  
//}  
//
//float anti_windup_PID_realize(float speed)  //抗饱和PID 
//{  
//    pid.Kp = 0.2;  
//    pid.Ki = 0.1;  
//    pid.Kd = 0.2;  
//    pid.umax = 400;  
//    pid.umin = -200;  
//    int index;  
//    pid.SetSpeed = speed;  
//    pid.err = pid.SetSpeed - pid.ActualSpeed;  
//    if (pid.ActualSpeed > pid.umax) {  
//        if (abs(pid.err) > 200)  
//        {  
//            index = 0;  
//        }  
//        else {  
//            index = 1;  
//            if (pid.err < 0)  
//            {  
//                pid.integral += pid.err;  
//            }  
//        }  
//    }  
//    else if (pid.ActualSpeed < pid.umin) {  
//        if (abs(pid.err) > 200)  
//        {  
//            index = 0;  
//        }  
//        else {  
//            index = 1;  
//            if (pid.err > 0)  
//            {  
//                pid.integral += pid.err;  
//            }  
//        }  
//    }  
//    else {  
//        if (abs(pid.err) > 200)  
//        {  
//            index = 0;  
//        }  
//        else {  
//            index = 1;  
//            pid.integral += pid.err;  
//        }  
//    }  
//    pid.voltage = pid.Kp*pid.err + index*pid.Ki*pid.integral + pid.Kd*(pid.err - pid.err_last);  
//    pid.err_last = pid.err;  
//    pid.ActualSpeed = pid.voltage*1.0;  
//    return pid.ActualSpeed;  
//}  