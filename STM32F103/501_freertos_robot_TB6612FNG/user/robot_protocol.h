#ifndef __ROBOT_PROTOVOL_H__
#define __ROBOT_PROTOVOL_H__

#include "stdint.h"
#include "uart.h"

#define R_PUTS(pbuf, len)   Uart_StrSend(pbuf, len)

#define R_CMD_HEADER        0x3AA3

#define R_FIX_LEN           0x05    //协议固定长度
#define R_CTRL_DATA_LEN     0x08    
#define R_PATH_DATA_LEN     0x0C
#define R_ONE_DATA_LEN      0x01

#define R_REPORT_TIME       40      //40ms上报

#define R_CAR_WIDE          237     //车身宽度，即两个轮子间距（mm）
#define R_CAR_L             70      //轮子直径70.5mmm
#define R_CAR_ENCODER_N     282     //563/2     //转一圈 563 脉冲
#define R_CAR_PI            3.14
      
/* 根据编码器返回的一个脉冲的count,算出当前速度v,注意count单位 
*   v(c) = PI*L/(N*c) = 3.14*70/500/c, 这里c是10的负5次方
*/

#define R_CAR_GET_V(count)  (39040852/(count))          //单位 um/s
#define R_CAR_Vr(v,w)       ((v)+((w)*R_CAR_WIDE)/2)    //右轮速度
#define R_CAR_Vl(v,w)       ((v)-((w)*R_CAR_WIDE)/2)    //左轮速度

#define R_CAR_V(Vr,Vl)      (((Vr)+(Vl))/2)                   //线速度 um/s
#define R_CAR_W(Vr,Vl)      (((Vr)-(Vl))/R_CAR_WIDE)   //角速度 10-3rad/s


#define R_CAR_DEG_W(w)      ((w)*573/100)               //deg_w = 360*rad/(2*PI)
#define R_CAR_DEG(deg_w)    (((deg_w)*(R_REPORT_TIME))/1000)     //deg = deg_w*40/1000    


#define R_BL_16(x)          ((uint16_t)((x)&0x00FFU)<<8)|((uint16_t)((x)&0xFF00U)>>8)
#define R_BL_32(x)          (((uint32_t)((x)&0x000000FFU)<<24)| \
                            (((uint32_t)((x)&0x0000FF00U)<<8))|\
                            (((uint32_t)((x)&0x00FF0000U)>>8))|\
                            (((uint32_t)((x)&0xFF000000U)>>24)))


enum Back_Type{
    R_SUCCESS = 0x00,     //正确执行
    R_FAILD   = 0x01,     //有错误重新发送
    R_STANDBY = 0x02,     //待机
    R_SLEEP   = 0x03      //休眠
};

enum Cmd_Type{
    R_CTRL_DOWN_CMD   = 0xA0,       //线速度，角速度
    R_CTRL_UP_CMD     = 0xB0,
    R_VS_DOWN_CMD     = 0xA1,       //线速度，距离
    R_VS_UP_CMD       = 0xB1,
    R_WD_DOWN_CMD     = 0xA2,
    R_WD_UP_CMD       = 0xB2,       //角速度，度
    R_PATH_REPORT_CMD = 0x10,
    R_MODE_DOWN_CMD   = 0x12,       //模式切换
    R_MODE_UP_CMD     = 0x22,
    
    R_FLAG_DOWN_CMD   = 0x00,       //自定义
    R_FLAG_UP_CMD     = 0x00,
};

typedef struct{
    int v;      //线速度 int 值，数据单位为微米每秒（um/s）
    int w;      //角速度 int 值，数据单位为毫弧度每秒（10-3 rad/s）
}Ctrl_Struct;

typedef struct{
    int v;
    int s;
}VS_Struct;

typedef struct{
    int w;
    int d;
}WD_Struct;

typedef struct{
    int v;      //线速度 int 值，数据单位为微米每秒（um/s）
    int w;      //角速度 int 值，数据单位为毫弧度每秒（10-3 rad/s）
    int16_t deg_w;//角速度 short 值，数据单位为百分之一度每秒（10-2 deg/s）   
    int16_t deg;  //角度 short 值，数据单位为百分之一度（10-2 deg）
}Path_Struct;

typedef struct{
    uint8_t mode;
}Mode_Struct;

typedef struct{
    uint8_t bcc;
}Flag_Struct;

//typedef union{
//    Ctrl_Struct ctrl;   //行走轨迹控制
//    Path_Struct path;   //底盘轨迹反馈
//}Data_Struct;
typedef struct{
    int Vr;
    int Vl;
}Speed_Struct;

typedef struct{
    uint16_t header;
    uint8_t  cmd_type;
    uint8_t  data_len;
    union{
        Ctrl_Struct ctrl;   //行走轨迹控制
        VS_Struct   vs;     //线速度，距离
        WD_Struct   wd;     //角速度，度
        Path_Struct path;   //底盘轨迹反馈
        Mode_Struct mode;   //模式切换
        Flag_Struct flag;   //自定义
        uint8_t     data;   //其他类型数据
    };
    uint8_t res;
}Protocol_Struct;

typedef struct{
    uint8_t cmd_type;
    uint8_t bcc_flag;   //1:off 0:on
    int v;  //down cmd 
    int w;  //down cmd 
    int Sr; //down cmd  单位 um
    int Sl; //down cmd  单位 um
//    Ctrl_Struct ctrl;   //行走轨迹控制
//    VS_Struct   vs;     //线速度，距离
//    WD_Struct   wd;     //角速度，度
    Path_Struct path;   //底盘轨迹反馈
    Mode_Struct mode;   //模式切换
    Speed_Struct cur;   //当前速度
    Speed_Struct dst;   //目的速度
    
}Protocol_Status_Struct;

extern Protocol_Status_Struct Protocol_Status;
extern char EncodeProcess(const uint8_t *buf);
extern void PathReport(void);

#endif  /* __ROBOT_PROTOVOL_H__ */