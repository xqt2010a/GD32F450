#ifndef __ROBOT_PROTOVOL_H__
#define __ROBOT_PROTOVOL_H__

#include "stdint.h"
#include "uart.h"

#define R_PUTS(pbuf, len)   Uart1_StrSend(pbuf, len)

#define R_CMD_HEADER        0x3AA3

#define R_FIX_LEN           0x05
#define R_CTRL_DATA_LEN     0x08
#define R_PATH_DATA_LEN     0x0C
#define R_OTHER_DATA_LEN    0x01

#define R_CAR_WIDE          237  //车身宽度，即两个轮子间距（mm）
#define R_CAR_Vr(v,w)       ((v)+((w)*R_CAR_WIDE)/2)    //右轮速度
#define R_CAR_Vl(v,w)       ((v)-((w)*R_CAR_WIDE)/2)    //左轮速度

#define R_CAR_V(r,l)        (((r)+(l))/2)               //线速度
#define R_CAR_W(r,l)        (((r)-(l))/2)               //角速度


enum Back_Type{
    R_SUCCESS = 0x00,     //正确执行
    R_FAILD   = 0x01,     //有错误重新发送
    R_STANDBY = 0x02,     //待机
    R_SLEEP   = 0x03      //休眠
};

enum Cmd_Type{
    R_CTRL_DOWN_CMD   = 0xA0,
    R_CTRL_UP_CMD     = 0xB0,
    R_PATH_REPORT_CMD = 0x10,
    R_MODE_DOWN_CMD   = 0x12,
    R_MODE_UP_CMD     = 0x22
};

typedef struct{
    int v;      //线速度 int 值，数据单位为微米每秒（um/s）
    int w;      //角速度 int 值，数据单位为毫弧度每秒（10-3 rad/s）
}Ctrl_Struct;

typedef struct{
    int v;      //线速度 int 值，数据单位为微米每秒（um/s）
    int w;      //角速度 int 值，数据单位为毫弧度每秒（10-3 rad/s）
    short deg_w;//角速度 short 值，数据单位为百分之一度每秒（10-2 deg/s）   
    short deg;  //角度 short 值，数据单位为百分之一度（10-2 deg）
}Path_Struct;

//typedef union{
//    Ctrl_Struct ctrl;   //行走轨迹控制
//    Path_Struct path;   //底盘轨迹反馈
//}Data_Struct;


typedef struct{
    uint16_t header;
    uint8_t  cmd_type;
    uint8_t  data_len;
    union{
        Ctrl_Struct ctrl;   //行走轨迹控制
        Path_Struct path;   //底盘轨迹反馈
        uint8_t     mode;   //模式切换
        uint8_t     data;   //其他类型数据
    };
    uint8_t res;
}Protocol_Struct;

typedef struct{
    uint8_t cmd_type;
    Ctrl_Struct ctrl;   //行走轨迹控制
    Path_Struct path;   //底盘轨迹反馈
    uint8_t     mode;   //模式切换
}Protocol_Status_Struct;


extern Protocol_Status_Struct Protocol_Status;
extern char EncodeProcess(const uint8_t *buf);
extern void PathReport(void);

#endif  /* __ROBOT_PROTOVOL_H__ */