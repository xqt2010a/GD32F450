#include "iic.h"

void IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_S;
    GPIO_S.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_S.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_S.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOB, &GPIO_S);
}

void IIC_WriteBytes(uint8_t device_addr, uint16_t addr, uint8_t *data, uint16_t len)
{
    uint16_t i;
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
    
    I2C_GenerateSTART(I2C1, ENABLE);    //START
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    
    I2C_Send7bitAddress(I2C1, device_addr, I2C_Direction_Transmitter);  //write device address
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    
#if(8 == WORD_ADDR_SIZE)
    I2C_SendData(I2C1, (uint8_t)(addr&0x00FF));         //addr = 8 bit
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
#else
    I2C_SendData(I2C1, (uint8_t)(addr>>8));         //addr = 16 bit
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    I2C_SendData(I2C1, (uint8_t)(addr&0x00FF));
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
#endif
    
    for(i=0; i<(len-1); i++){
        I2C_SendData(I2C1, *data++);   //write data
        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    }
    
    I2C_SendData(I2C1, *data);   //write data
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    
    I2C_GenerateSTOP(I2C1, ENABLE);     //stop
}

void IIC_ReadBytes(uint8_t device_addr, uint16_t addr, uint8_t *data, uint16_t len)
{
    uint16_t i;
    I2C_GenerateSTART(I2C1, ENABLE);    //START
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    
    I2C_Send7bitAddress(I2C1, device_addr, I2C_Direction_Transmitter);  //write device address
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    
#if(8 == WORD_ADDR_SIZE)
    I2C_SendData(I2C1, (uint8_t)(addr&0x00FF));         //addr = 8 bit
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
#else
    I2C_SendData(I2C1, (uint8_t)(addr>>8));         //addr = 16 bit
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    I2C_SendData(I2C1, (uint8_t)(addr&0x00FF));
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
#endif
    I2C_GenerateSTART(I2C1, ENABLE);    //reSTART
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    
    I2C_Send7bitAddress(I2C1, device_addr, I2C_Direction_Transmitter);  //write device address ready to read
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
    
    
    for(i=0; i<(len-1); i++){
        I2C_AcknowledgeConfig(I2C1, ENABLE);   //产生应答
        while(I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET);
        *data++ = I2C_ReceiveData(I2C1);      //read data
    }
    
    I2C_AcknowledgeConfig(I2C1, DISABLE);   //产生非应答
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET);
    *data = I2C_ReceiveData(I2C1);          //read data
    
    I2C_GenerateSTOP(I2C1, ENABLE);     //stop
}