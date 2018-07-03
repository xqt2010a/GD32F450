#ifndef __ROBOT_PROTOVOL_H__
#define __ROBOT_PROTOVOL_H__

#include "stdint.h"
#include "uart.h"

#define ROBOT_DEBUG         0

#define R_PUTS(pbuf, len)   Uart_StrSend(pbuf, len)

#define R_CMD_HEADER        0x3AA3

#define R_FIX_LEN           0x05    //Э��̶�����
#define R_CTRL_DATA_LEN     0x08    
#define R_PATH_DATA_LEN     0x0C
#define R_ONE_DATA_LEN      0x01

#define R_REPORT_TIME       40      //40ms�ϱ�

#define R_CAR_WIDE          234000//237     //�����ȣ����������Ӽ�ࣨmm��
#define R_CAR_LEN_R         70600//70530      //����ֱ��70.5mmm
#define R_CAR_LEN_L         71080//71000         
#define R_CAR_ENCODER_N     563     //563/2     //תһȦ 563 ����
#define R_CAR_PI            314

#define STOP_LEN            250000//178194//450 ��ǰ�p�پ���um
#define MIN_SPEED           80000       //��С�ٶ�
#define MAX_SPEED           300000
      
#define PERIOD_BUFSIZE      20

/* ���ݱ��������ص�һ�������count,�����ǰ�ٶ�v,ע��count��λ 
*   v(c) = PI*L/(N*c) = 3.14*R_CAR_LEN/R_CAR_ENCODER_N/c, ����c��10�ĸ�5�η�
*/

#define R_CAR_GET_Vl(count)  39643197/(count)//((R_CAR_PI)*(R_CAR_LEN_L)*1000/(R_CAR_ENCODER_N*count)) //39623801 --- 71
#define R_CAR_GET_Vr(count)  39375489/(count)//((R_CAR_PI)*(R_CAR_LEN_R)*1000/(R_CAR_ENCODER_N*count))         //(39040852/(count))  ---- 70    //��λ um/s
#define R_CAR_Vr(v,w)       ((v)+((w)*R_CAR_WIDE)/2000)    //�����ٶ�
#define R_CAR_Vl(v,w)       ((v)-((w)*R_CAR_WIDE)/2000)    //�����ٶ�

#define R_CAR_V(Vr,Vl)      (((Vr)+(Vl))/2)                   //���ٶ� um/s
#define R_CAR_W(Vr,Vl)      (((Vr)-(Vl))*1000/R_CAR_WIDE)   //���ٶ� 10-3rad/s


#define R_CAR_DEG_W(w)      ((w)*573/100)               //deg_w = 360*rad/(2*R_CAR_PI)
#define R_CAR_DEG(deg_w)    (((deg_w)*(R_REPORT_TIME))/1000)     //deg = deg_w*40/1000    


#define ABS_FUC(x)  ((x)>=0?(x):(-(x)))

#define ZOOM_RATE            10000

#define R_BL_16(x)          ((uint16_t)((x)&0x00FFU)<<8)|((uint16_t)((x)&0xFF00U)>>8)
#define R_BL_32(x)          (((uint32_t)((x)&0x000000FFU)<<24)| \
                            (((uint32_t)((x)&0x0000FF00U)<<8))|\
                            (((uint32_t)((x)&0x00FF0000U)>>8))|\
                            (((uint32_t)((x)&0xFF000000U)>>24)))


enum Back_Type{
    R_SUCCESS = 0x00,     //��ȷִ��
    R_FAILD   = 0x01,     //�д������·���
    R_STANDBY = 0x02,     //����
    R_SLEEP   = 0x03      //����
};

enum Cmd_Type{
    R_CTRL_DOWN_CMD   = 0xA0,       //���ٶȣ����ٶ�
    R_CTRL_UP_CMD     = 0xB0,
    R_VS_DOWN_CMD     = 0xA1,       //���ٶȣ�����
    R_VS_UP_CMD       = 0xB1,
    R_WD_DOWN_CMD     = 0xA2,       //���ٶȣ���
    R_WD_UP_CMD       = 0xB2,       
    R_PATH_REPORT_CMD = 0x10,
    R_MODE_DOWN_CMD   = 0x12,       //ģʽ�л�
    R_MODE_UP_CMD     = 0x22,
    
    R_FLAG_DOWN_CMD   = 0x00,       //�Զ���
    R_FLAG_UP_CMD     = 0x00,
};

typedef struct{
    int v;      //���ٶ� int ֵ�����ݵ�λΪ΢��ÿ�루um/s��
    int w;      //���ٶ� int ֵ�����ݵ�λΪ������ÿ�루10-3 rad/s��
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
    int v;      //���ٶ� int ֵ�����ݵ�λΪ΢��ÿ�루um/s��
    int w;      //���ٶ� int ֵ�����ݵ�λΪ������ÿ�루10-3 rad/s��
    int16_t deg_w;//���ٶ� short ֵ�����ݵ�λΪ�ٷ�֮һ��ÿ�루10-2 deg/s��   
    int16_t deg;  //�Ƕ� short ֵ�����ݵ�λΪ�ٷ�֮һ�ȣ�10-2 deg��
}Path_Struct;

typedef struct{
    uint8_t mode;
}Mode_Struct;

typedef struct{
    uint8_t bcc;
}Flag_Struct;

typedef struct{
    int Vr;
    int Vl;
}Speed_Struct;

typedef struct{
    uint32_t r;
    uint32_t l;
}U32RL_Struct;

typedef struct{     //��ʱ������ʱ��
    uint16_t buf_r[PERIOD_BUFSIZE];
    uint16_t buf_l[PERIOD_BUFSIZE];
    uint8_t index_r;
    uint8_t index_l;
}Capture_Struct;

typedef struct{
    uint16_t header;
    uint8_t  cmd_type;
    uint8_t  data_len;
    union{
        Ctrl_Struct ctrl;   //���߹켣����
        VS_Struct   vs;     //���ٶȣ�����
        WD_Struct   wd;     //���ٶȣ���
        Path_Struct path;   //���̹켣����
        Mode_Struct mode;   //ģʽ�л�
        Flag_Struct flag;   //�Զ���
        uint8_t     data;   //������������
    };
    uint8_t res;
}Protocol_Struct;

typedef struct{
    uint8_t cmd_type;
    uint8_t bcc_flag;           //1:off 0:on
    int v;                      //down cmd 
    int w;                      //down cmd 
    int last_Vl;                //��¼��һ�ε� Vl
    int last_Vr; 
    U32RL_Struct count;         //��¼����������count
    U32RL_Struct dst_s;         //Ŀ�����߾���
    U32RL_Struct cur_s;         //��ǰ���߾���
    U32RL_Struct num;           //��¼�ϱ�֡��

    Path_Struct path;           //���̹켣����
    Mode_Struct mode;           //ģʽ�л�
    Speed_Struct cur_v;         //��ǰ�ٶ�
    Speed_Struct dst_v;         //Ŀ���ٶ�
    Speed_Struct dst_temp_v;    //��ʱ�洢
    Capture_Struct cap;         //��������������ʱ��
}Protocol_Status_Struct;


extern Protocol_Status_Struct Protocol_Status;

extern void Protocol_Init(void);
extern char EncodeProcess(const uint8_t *buf);
extern void PathReport(void);

#endif  /* __ROBOT_PROTOVOL_H__ */