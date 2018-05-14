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

#define R_CAR_WIDE          237  //�����ȣ����������Ӽ�ࣨmm��
#define R_CAR_Vr(v,w)       ((v)+((w)*R_CAR_WIDE)/2)    //�����ٶ�
#define R_CAR_Vl(v,w)       ((v)-((w)*R_CAR_WIDE)/2)    //�����ٶ�

#define R_CAR_V(r,l)        (((r)+(l))/2)               //���ٶ�
#define R_CAR_W(r,l)        (((r)-(l))/2)               //���ٶ�


enum Back_Type{
    R_SUCCESS = 0x00,     //��ȷִ��
    R_FAILD   = 0x01,     //�д������·���
    R_STANDBY = 0x02,     //����
    R_SLEEP   = 0x03      //����
};

enum Cmd_Type{
    R_CTRL_DOWN_CMD   = 0xA0,
    R_CTRL_UP_CMD     = 0xB0,
    R_PATH_REPORT_CMD = 0x10,
    R_MODE_DOWN_CMD   = 0x12,
    R_MODE_UP_CMD     = 0x22
};

typedef struct{
    int v;      //���ٶ� int ֵ�����ݵ�λΪ΢��ÿ�루um/s��
    int w;      //���ٶ� int ֵ�����ݵ�λΪ������ÿ�루10-3 rad/s��
}Ctrl_Struct;

typedef struct{
    int v;      //���ٶ� int ֵ�����ݵ�λΪ΢��ÿ�루um/s��
    int w;      //���ٶ� int ֵ�����ݵ�λΪ������ÿ�루10-3 rad/s��
    short deg_w;//���ٶ� short ֵ�����ݵ�λΪ�ٷ�֮һ��ÿ�루10-2 deg/s��   
    short deg;  //�Ƕ� short ֵ�����ݵ�λΪ�ٷ�֮һ�ȣ�10-2 deg��
}Path_Struct;

//typedef union{
//    Ctrl_Struct ctrl;   //���߹켣����
//    Path_Struct path;   //���̹켣����
//}Data_Struct;


typedef struct{
    uint16_t header;
    uint8_t  cmd_type;
    uint8_t  data_len;
    union{
        Ctrl_Struct ctrl;   //���߹켣����
        Path_Struct path;   //���̹켣����
        uint8_t     mode;   //ģʽ�л�
        uint8_t     data;   //������������
    };
    uint8_t res;
}Protocol_Struct;

typedef struct{
    uint8_t cmd_type;
    Ctrl_Struct ctrl;   //���߹켣����
    Path_Struct path;   //���̹켣����
    uint8_t     mode;   //ģʽ�л�
}Protocol_Status_Struct;


extern Protocol_Status_Struct Protocol_Status;
extern char EncodeProcess(const uint8_t *buf);
extern void PathReport(void);

#endif  /* __ROBOT_PROTOVOL_H__ */