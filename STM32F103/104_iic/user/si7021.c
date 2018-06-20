#include "si7021.h"

#define DEVICE_ADDR         0x80
#define MEASURE_T_HOLD      0xE3
#define MEASURE_RH_HOLD     0xE5

uint8_t Rx_Buf[2];

void SI7021_Init(void)
{
    IIC_Init();
}

uint32_t Read_T(void)
{
    uint32_t r_value,temp;
    IIC_ReadBytes(DEVICE_ADDR, MEASURE_T_HOLD, Rx_Buf, 2);
    temp = (Rx_Buf[0]<<8)|(Rx_Buf[0]);
    r_value = 17572*temp/65536-4685;
    return r_value;
}

uint32_t Read_RH(void)
{
    uint32_t r_value,temp;
    IIC_ReadBytes(DEVICE_ADDR, MEASURE_RH_HOLD, Rx_Buf, 2);
    temp = (Rx_Buf[0]<<8)|(Rx_Buf[0]);
    r_value = 125*temp/65536-600;
    return r_value;
}
