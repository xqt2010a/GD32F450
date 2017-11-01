/**
******************************************************************************
* @file    bsp_led.c
* @author  fire
* @version V1.0
* @date    2013-xx-xx
* @brief   ledӦ�ú����ӿ�
******************************************************************************
* @attention
*
* ʵ��ƽ̨:Ұ�� iSO STM32 ������ 
* ��̳    :http://www.chuxue123.com
* �Ա�    :http://firestm32.taobao.com
*
******************************************************************************
*/
#include "bsp_led.h"   
#include "hw_config.h"

/**
* @brief  ��ʼ������LED��IO
* @param  ��
* @retval ��
*/
void LED_GPIO_Config(void)
{		
    /*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /*����GPIOB��GPIOF������ʱ��*/
    //RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOF, ENABLE);
    
    /*����GPIOC������ʱ��*/
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE);
    /* Enable GPIOA, GPIOD and USART1 clock */
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    
    /*ѡ��Ҫ���Ƶ�GPIOC����*/															   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	
    
    /*��������ģʽΪͨ���������*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
    
    /*������������Ϊ50MHz */   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    
    /*���ÿ⺯������ʼ��GPIOC0*/
    GPIO_Init(GPIOC, &GPIO_InitStructure);	
    
    /*ѡ��Ҫ���Ƶ�GPIOF����*/															   
    //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    
    /*���ÿ⺯������ʼ��GPIOF7*/
    //GPIO_Init(GPIOF, &GPIO_InitStructure);
    
    /*ѡ��Ҫ���Ƶ�GPIOF����*/															   
    //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    
    /*���ÿ⺯������ʼ��GPIOF7*/
    //GPIO_Init(GPIOF, &GPIO_InitStructure);			  
    
    /* �ر�����led��	*/
    //GPIO_SetBits(GPIOC, GPIO_Pin_13);  /* �� PC13 */
    GPIO_ResetBits(GPIOC, GPIO_Pin_13);  /* �� PC13 */
    /* �ر�����led��	*/
    //GPIO_SetBits(GPIOF, GPIO_Pin_7|GPIO_Pin_8);	
    
    
    
    /* PD.09 used as USB pull-up */
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
//    GPIO_Init(GPIOD, &GPIO_InitStructure); 
}
/*********************************************END OF FILE**********************/
