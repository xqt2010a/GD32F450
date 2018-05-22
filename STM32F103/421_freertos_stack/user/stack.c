#include "stack.h"

#define timerINTERRUPT_FREQUENCY    20000
#define timerHIGHEST_PRIORITY       4

uint32_t ulHighFrequencyTimerTicks = 0;

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    PRT("task: %s stack overflow\r\n", pcTaskName);
}

void Stack_Timer_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_S;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    TIM_S.TIM_Period = 60000;//((SystemCoreClock)/timerINTERRUPT_FREQUENCY - 1);  /* 自动重装 */
    TIM_S.TIM_Prescaler = 720-1;    /* 分频比 = 1 */
    TIM_S.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_S.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_S.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM4, &TIM_S);
    TIM_ARRPreloadConfig(TIM4, ENABLE);

    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM4, ENABLE);
    
    NVIC_InitTypeDef NVIC_S;
    NVIC_S.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_S.NVIC_IRQChannelPreemptionPriority = timerHIGHEST_PRIORITY;
    NVIC_S.NVIC_IRQChannelSubPriority = 0;
    NVIC_S.NVIC_IRQChannelCmd = ENABLE;  
    NVIC_Init(&NVIC_S);
}

void vTask_PRT(void *pvParameters)
{
    uint8_t wbuf[100];
    Stack_Timer_Init();
    while(1){
        PRT("=============================================\r\n");
        PRT("task       state   priority    surplus     number\r\n");
        vTaskList((char *)&wbuf);
        PRT("%s\r\n",wbuf);
        
        PRT("\r\ntask   count   rate\r\n");
        vTaskGetRunTimeStats((char *)wbuf);
        PRT("%s\r\n",wbuf);
        
        vTaskDelay(1000);
    }
}

void TIM4_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET){
        ulHighFrequencyTimerTicks++;
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    }
}