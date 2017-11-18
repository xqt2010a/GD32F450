#include "led.h"
#include "stm32f10x_rcc.h"

void Led_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    /* Enable the GPIO_LED Clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    
    /* Configure the GPIO_LED pin */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void Led_On(void)
{
    GPIO_ResetBits(GPIOC, GPIO_Pin_13);  /* ¿ª PC13 */    
}

void Led_Off(void)
{
    GPIO_SetBits(GPIOC, GPIO_Pin_13);  /* ¹Ø PC13 */    
}

void Led_Blink(void)
{
    uint32_t i = 0;
    Led_On();
    for(i=0; i<0xFFFFF; i++);
    Led_Off();
    for(i=0; i<0xFFFFF; i++);
}