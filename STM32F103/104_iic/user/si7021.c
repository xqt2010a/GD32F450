#include "si7021.h"

#define DEVICE_ADDR         0x80
#define MEASURE_T_HOLD      0xE3
#define MEASURE_RH_HOLD     0xE5
#define MEASURE_T_NOHOLD    0xF3
#define MEASURE_RH_NOHOLD   0xF5


uint8_t Rx_Buf[2];

void SI7021_Init(void)
{
    IIC_Init();
}

uint32_t Read_T(void)
{
    uint32_t temp;
    int32_t r_value;
    //IIC_ReadBytes(DEVICE_ADDR, MEASURE_T_HOLD, Rx_Buf, 2);
    IIC_Start();
    IIC_WriteByte(DEVICE_ADDR);         //write
    IIC_WaitAck();
    IIC_WriteByte(MEASURE_T_NOHOLD);    //cmd
    IIC_WaitAck();
    delay_us(20000);  //delay 20ms for transformation
    IIC_Start();
    IIC_WriteByte(DEVICE_ADDR|0x01);    //read
    IIC_WaitAck();
    Rx_Buf[0] = IIC_ReadByte(1);
    Rx_Buf[1] = IIC_ReadByte(0);
    IIC_Stop();
    temp = (Rx_Buf[0]<<8)|(Rx_Buf[0]);
    r_value = 17572*temp/65536-4685;
    return r_value;
}

uint32_t Read_RH(void)
{
    uint32_t temp;
    int32_t r_value;
    //IIC_ReadBytes(DEVICE_ADDR, MEASURE_RH_HOLD, Rx_Buf, 2);
    IIC_Start();
    IIC_WriteByte(DEVICE_ADDR);         //write
    IIC_WaitAck();
    IIC_WriteByte(MEASURE_RH_NOHOLD);    //cmd
    IIC_WaitAck();
    delay_us(20000);  //delay 20ms for transformation
    IIC_Start();
    IIC_WriteByte(DEVICE_ADDR|0x01);    //read
    IIC_WaitAck();
    Rx_Buf[0] = IIC_ReadByte(1);
    Rx_Buf[1] = IIC_ReadByte(0);
    IIC_Stop();
    temp = (Rx_Buf[0]<<8)|(Rx_Buf[0]);
    r_value = 12500*temp/65536-600;
    return r_value;
}
