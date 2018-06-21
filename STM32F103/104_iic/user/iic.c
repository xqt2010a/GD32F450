#include "iic.h"

void delay_us(uint32_t t)
{
    uint32_t i,j;
    for(i=0; i<t; i++){
        for(j=0; j<0xF; j++);
    }
}

void SDA_OUT(void)
{  
    GPIO_InitTypeDef GPIO_InitTypeStruct;  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);  
      
    GPIO_InitTypeStruct.GPIO_Mode=GPIO_Mode_Out_PP;  
    GPIO_InitTypeStruct.GPIO_Pin=GPIO_Pin_6;  
    GPIO_InitTypeStruct.GPIO_Speed=GPIO_Speed_50MHz;  
    GPIO_Init(GPIOB,&GPIO_InitTypeStruct);  
}

void SDA_IN(void)
{  
    GPIO_InitTypeDef GPIO_InitTypeStruct;  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);  
      
    GPIO_InitTypeStruct.GPIO_Mode=GPIO_Mode_IPU;//GPIO_Mode_IN_FLOATING;  
    GPIO_InitTypeStruct.GPIO_Pin=GPIO_Pin_6;  
    GPIO_InitTypeStruct.GPIO_Speed=GPIO_Speed_50MHz;  
    GPIO_Init(GPIOB,&GPIO_InitTypeStruct);  
}  

void IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_S;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    GPIO_S.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_S.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_S.GPIO_Mode = GPIO_Mode_Out_PP;//GPIO_Mode_AF_OD;
    GPIO_Init(GPIOB, &GPIO_S);
    
    GPIO_SetBits(GPIOB, GPIO_Pin_6|GPIO_Pin_7);
}

void IIC_Start(void)
{
    SDA_OUT();
    IIC_SDA(Bit_SET);
    IIC_SCL(Bit_SET);
    delay_us(4);
    IIC_SDA(Bit_RESET);
    delay_us(4);
    IIC_SCL(Bit_RESET);
}

void IIC_Stop(void)
{
    SDA_OUT();
    IIC_SCL(Bit_RESET);
    IIC_SDA(Bit_SET);
    delay_us(4);
    IIC_SCL(Bit_SET);
    IIC_SDA(Bit_SET);
    delay_us(4);
}

uint8_t IIC_WaitAck(void)
{
    uint8_t t;
    SDA_IN();
    IIC_SDA(Bit_SET);
    delay_us(2);
    IIC_SCL(Bit_SET);
    delay_us(2);
    while(READ_SDA){
        t++;
        if(t > 250){
            IIC_Stop();
            return 1;   //timeout faild
        }
        delay_us(2);
    }
    IIC_SCL(Bit_RESET);
    //delay_us(2);
    return 0;   //success
}

void IIC_Ack(void)
{
    IIC_SCL(Bit_RESET);
    SDA_OUT();
    IIC_SDA(Bit_RESET);
    delay_us(2);
    IIC_SCL(Bit_SET);
    delay_us(2);
    IIC_SCL(Bit_RESET);
}

void IIC_NAck(void)
{
    IIC_SCL(Bit_RESET);
    SDA_OUT();
    IIC_SDA(Bit_SET);
    delay_us(2);
    IIC_SCL(Bit_SET);
    delay_us(2);
    IIC_SCL(Bit_RESET);
}

void IIC_WriteByte(uint8_t data)
{
    uint8_t i;
    SDA_OUT();
    IIC_SCL(Bit_RESET);
    for(i=0; i<8; i++){
        IIC_SDA(((data&0x80)>>7)?Bit_SET:Bit_RESET);
        data<<=1;
        delay_us(2);
        IIC_SCL(Bit_SET);
        delay_us(2);
        IIC_SCL(Bit_RESET);
        delay_us(2);
    }
//    delay_us(2);
//    IIC_SCL(Bit_SET);
//    delay_us(2);
//    IIC_SCL(Bit_RESET);
//    delay_us(2);
}

//read 1 Byte, ack = 1,send ACK; ack = 0, send NAck. 
uint8_t IIC_ReadByte(uint8_t ack)
{
    uint8_t i, rx=0;
    SDA_IN();
    for(i=0; i<8; i++){
        IIC_SCL(Bit_RESET);
        delay_us(2);
        IIC_SCL(Bit_SET);
        rx<<=1;
        if(READ_SDA)
            rx++;
        delay_us(2);
    }
    if(!ack)
        IIC_NAck();
    else
        IIC_Ack();
    return rx;
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