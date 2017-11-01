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
  * ʵ��ƽ̨:Ұ�� iSO STM32 ������ 
  * ��̳    :http://www.chuxue123.com
  * �Ա�    :http://firestm32.taobao.com
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
	Init_System();//ϵͳ��ʼ��
	
	LED_GPIO_Config(); //LED��USB�Ƿ�����

	//USART1_Config(); //����1����

	Set_USBClock();//����USBʱ��
	
	USB_Interrupts_Config(); //����USB�ж�
	
	USB_Init();//��ʼ��USB   
	
    SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
    PRT("hello world!\r\n");
	while (1)
	{	 
		//�ж��������ݽ��յ�
//		if (count_out != 0)	
//		{    
//			//������յ��������0X01������led1  
//			if(buffer_out[0]==0x01)
//			{
//				GPIO_ResetBits(GPIOB, GPIO_Pin_0 );
//				USB_Send_Data("LED1a ����,labc\r\n");
//				count_out=0;
//			}
//
//			//������յ��������0X02������led2	
//			if(buffer_out[0]==0x02)
//			{
//				GPIO_ResetBits(GPIOF, GPIO_Pin_7 );
//				USB_Send_Data("LED2a ����,labc\r\n");
//				count_out=0;
//			}
//				
//			//������յ��������0X03������led3
//			if(buffer_out[0]==0x03)
//			{
//				GPIO_ResetBits(GPIOF, GPIO_Pin_8 );
//				USB_Send_Data("LED3a ����,labc\r\n");
//				count_out=0;
//			}
//				
//			//������յ��������0X04��//�ر�3��LED
//			if(buffer_out[0]==0x04)
//			{
//				GPIO_SetBits(GPIOB, GPIO_Pin_0);
//				GPIO_SetBits(GPIOF, GPIO_Pin_7 | GPIO_Pin_8);
//				USB_Send_Data("LEDa ȫ��,labc\r\n");
//				count_out=0;
//			}   
//		}	
	}
}

