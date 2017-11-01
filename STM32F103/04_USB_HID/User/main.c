/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   usb mass storage 
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */

#include "stm32f10x_lib.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "hw_config.h"
#include "bsp_usart1.h"
#include "bsp_led.h"
#include "SEGGER_RTT.h"


#define PRT(...) SEGGER_RTT_printf(0, __VA_ARGS__)

extern u32 count_out;
extern u8 buffer_out[HID_PORT_DATA_SIZE];

int main(void)
{
	Init_System();//系统初始化
	
	LED_GPIO_Config(); //LED及USB是否上拉

	//USART1_Config(); //串口1配置

	Set_USBClock();//设置USB时钟
	
	USB_Interrupts_Config(); //配置USB中断
	
	USB_Init();//初始化USB   
	
    SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
    PRT("hello world!\r\n");
	while (1)
	{	 
		//判断有无数据接收到
//		if (count_out != 0)	
//		{    
//			//如果接收到的输出是0X01，点亮led1  
//			if(buffer_out[0]==0x01)
//			{
//				GPIO_ResetBits(GPIOB, GPIO_Pin_0 );
//				USB_Send_Data("LED1a 点亮,labc\r\n");
//				count_out=0;
//			}
//
//			//如果接收到的输出是0X02，点亮led2	
//			if(buffer_out[0]==0x02)
//			{
//				GPIO_ResetBits(GPIOF, GPIO_Pin_7 );
//				USB_Send_Data("LED2a 点亮,labc\r\n");
//				count_out=0;
//			}
//				
//			//如果接收到的输出是0X03，点亮led3
//			if(buffer_out[0]==0x03)
//			{
//				GPIO_ResetBits(GPIOF, GPIO_Pin_8 );
//				USB_Send_Data("LED3a 点亮,labc\r\n");
//				count_out=0;
//			}
//				
//			//如果接收到的输出是0X04，//关闭3个LED
//			if(buffer_out[0]==0x04)
//			{
//				GPIO_SetBits(GPIOB, GPIO_Pin_0);
//				GPIO_SetBits(GPIOF, GPIO_Pin_7 | GPIO_Pin_8);
//				USB_Send_Data("LEDa 全灭,labc\r\n");
//				count_out=0;
//			}   
//		}	
	}
}

