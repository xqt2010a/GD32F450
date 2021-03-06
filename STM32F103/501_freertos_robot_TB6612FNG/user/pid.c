#include "pid.h"
#include "uart.h"
#include "string.h"
#include "robot_protocol.h"
#include "SEGGER_RTT.h"

#define PID_KP_R  20     //0.2 = 200/1000
#define PID_KI_R  50      
#define PID_KD_R  10

#define PID_KP_L  20     //0.2 = 200/1000
#define PID_KI_L  50      
#define PID_KD_L  10

#define PID_KP_C  5    //calc = 120左右有较好的调节
#define PID_KI_C  20
#define PID_KD_C  5


uint8_t pid_buf[PID_BUF_LEN]={0xEE,0xEE,0xEE, 0xEE,0x00};


int32_t PID_realize_R(int32_t dst_v, int32_t cur_v, uint32_t count)     //增量式PID
{
    int32_t err, increment;//, calc;
    static int32_t err_t2 = 0, err_t1 = 0;

    err = dst_v - cur_v;
    increment = PID_KP_R*(err - err_t1) + PID_KI_R*err + PID_KD_R*(err - 2*err_t1 + err_t2);
    //calc = cur_v + increment/1000;
    pid_buf[4] = 0x01;      //右
    memcpy(pid_buf+5, (uint8_t *)&Protocol_Status.num.r, 4);
    memcpy(pid_buf+9, (uint8_t *)&Protocol_Status.dst_v.Vr, 4);
    memcpy(pid_buf+13, (uint8_t *)&Protocol_Status.cur_v.Vr, 4);
    memcpy(pid_buf+17, (uint8_t *)&err, 4);
    memcpy(pid_buf+21, (uint8_t *)&err_t1, 4);
    memcpy(pid_buf+25,(uint8_t *)&err_t2, 4);
    memcpy(pid_buf+29,(uint8_t *)&increment, 4);
    memcpy(pid_buf+33,(uint8_t *)&count, 4);
#if ROBOT_DEBUG
    Uart_StrSend(pid_buf, PID_BUF_LEN);
#endif  /* ROBOT_DEBUG */
    Protocol_Status.num.r++;
    
    err_t2 = err_t1;
    err_t1 = err;
    //return calc;
    return increment;
}

int32_t PID_realize_L(int32_t dst_v, int32_t cur_v, uint32_t count)     //增量式PID
{
    int32_t err, increment;//, calc;
    static int32_t err_t2 = 0, err_t1 = 0;
    
    err = dst_v - cur_v;
    increment = PID_KP_L*(err - err_t1) + PID_KI_L*err + PID_KD_L*(err - 2*err_t1 + err_t2);
    //calc = cur_v + increment/1000;
    pid_buf[4] = 0x02;      //左
    memcpy(pid_buf+5, (uint8_t *)&Protocol_Status.num.l, 4);
    memcpy(pid_buf+9, (uint8_t *)&Protocol_Status.dst_v.Vl, 4);
    memcpy(pid_buf+13,(uint8_t *)&Protocol_Status.cur_v.Vl, 4);
    memcpy(pid_buf+17, (uint8_t *)&err, 4);
    memcpy(pid_buf+21, (uint8_t *)&err_t1, 4);
    memcpy(pid_buf+25,(uint8_t *)&err_t2, 4);
    memcpy(pid_buf+29,(uint8_t *)&increment, 4);
    memcpy(pid_buf+33,(uint8_t *)&count, 4);
#if ROBOT_DEBUG
    Uart_StrSend(pid_buf, PID_BUF_LEN);
#endif  /* ROBOT_DEBUG */
    Protocol_Status.num.l++;
    
    err_t2 = err_t1;
    err_t1 = err;
    //return calc;
    return increment;
}

int32_t PID_Correct_Len(uint32_t dst, uint32_t cur)
{
    int32_t err, calc;
    static int32_t err_t1 = 0, err_t2 = 0;

    err = dst - cur;
    err /= 392;
    calc = PID_KP_C*(err-err_t1) + PID_KI_C*err + PID_KD_C*(err-2*err_t1+err_t2);
    err_t2 = err_t1;
    err_t1 = err;
    
    return calc;
}



//void PID_init()
//{
//    //printf("PID_init begin \n");
//    pid.SetSpeed = 0.0;  
//    pid.ActualSpeed = 0.0;  
//    pid.err = 0.0;  
//    pid.err_t2 = 0.0;  
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
//    pid.voltage = pid.Kp*pid.err + pid.Ki*pid.integral + pid.Kd*(pid.err - pid.err_t2);  
//    pid.err_t2 = pid.err;  
//    pid.ActualSpeed = pid.voltage*1.0;  
//    return pid.ActualSpeed;  
//}
//
//float incremental_PID_realize(float speed) {    //增量式PID
//    pid.err_t1 = 0.0;  
//  
//    pid.SetSpeed = speed;  
//    pid.err = pid.SetSpeed - pid.ActualSpeed;  
//    float incrementSpeed = pid.Kp*(pid.err - pid.err_t1) + pid.Ki*pid.err + pid.Kd*(pid.err - 22 * pid.err_t1 + pid.err_t2);  
//    pid.ActualSpeed += incrementSpeed;  
//    pid.err_t2 = pid.err_t1;  
//    pid.err_t1 = pid.err;  
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
//    pid.voltage = pid.Kp*pid.err + index*pid.Ki*pid.integral + pid.Kd*(pid.err - pid.err_t2);  
//    pid.err_t2 = pid.err;  
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
//    pid.voltage = pid.Kp*pid.err + index*pid.Ki*pid.integral + pid.Kd*(pid.err - pid.err_t2);  
//    pid.err_t2 = pid.err;  
//    pid.ActualSpeed = pid.voltage*1.0;  
//    return pid.ActualSpeed;  
//}  