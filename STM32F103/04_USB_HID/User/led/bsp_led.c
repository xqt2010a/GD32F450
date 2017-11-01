/**
******************************************************************************
* @file    bsp_led.c
* @author  fire
* @version V1.0
* @date    2013-xx-xx
* @brief   led应用函数接口
******************************************************************************
* @attention
*
* 实验平台:野火 iSO STM32 开发板 
* 论坛    :http://www.chuxue123.com
* 淘宝    :http://firestm32.taobao.com
*
******************************************************************************
*/
#include "bsp_led.h"   
#include "hw_config.h"

/**
* @brief  初始化控制LED的IO
* @param  无
* @retval 无
*/
void LED_GPIO_Config(void)
{		
    /*定义一个GPIO_InitTypeDef类型的结构体*/
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /*开启GPIOB和GPIOF的外设时钟*/
    //RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOF, ENABLE);
    
    /*开启GPIOC的外设时钟*/
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE);
    /* Enable GPIOA, GPIOD and USART1 clock */
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    
    /*选择要控制的GPIOC引脚*/															   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	
    
    /*设置引脚模式为通用推挽输出*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
    
    /*设置引脚速率为50MHz */   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    
    /*调用库函数，初始化GPIOC0*/
    GPIO_Init(GPIOC, &GPIO_InitStructure);	
    
    /*选择要控制的GPIOF引脚*/															   
    //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    
    /*调用库函数，初始化GPIOF7*/
    //GPIO_Init(GPIOF, &GPIO_InitStructure);
    
    /*选择要控制的GPIOF引脚*/															   
    //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    
    /*调用库函数，初始化GPIOF7*/
    //GPIO_Init(GPIOF, &GPIO_InitStructure);			  
    
    /* 关闭所有led灯	*/
    //GPIO_SetBits(GPIOC, GPIO_Pin_13);  /* 关 PC13 */
    GPIO_ResetBits(GPIOC, GPIO_Pin_13);  /* 开 PC13 */
    /* 关闭所有led灯	*/
    //GPIO_SetBits(GPIOF, GPIO_Pin_7|GPIO_Pin_8);	
    
    
    
    /* PD.09 used as USB pull-up */
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
//    GPIO_Init(GPIOD, &GPIO_InitStructure); 
}
/*********************************************END OF FILE**********************/
