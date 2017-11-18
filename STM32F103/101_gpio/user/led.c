#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "led.h"

//=============================================================================
//name:     Led_Init
//function: Init GPIOC_13 for led
//parameter:no
//return:   no
//=============================================================================

void Led_Init(void)
{
    GPIO_InitTypeDef GPIO_CS;
    GPIO_CS.GPIO_Pin = GPIO_Pin_13;
    GPIO_CS.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_CS.GPIO_Mode = GPIO_Mode_Out_PP;
    
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC , ENABLE);  /* enable rcc */
    GPIO_Init(GPIOC, &GPIO_CS);  /* init PC13 */
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