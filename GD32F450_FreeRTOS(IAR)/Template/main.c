/*!
    \file  main.c
    \brief led spark with systick 
*/

/*
    Copyright (C) 2016 GigaDevice

    2016-08-15, V1.0.0, firmware for GD32F4xx
*/
#include "FreeRTOS.h"
#include "task.h"
#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "gd32f4xx_eval.h"

/*!
    \brief      toggle the led every 500ms
    \param[in]  none
    \param[out] none
    \retval     none
*/
void DemoTask(void *pvParameters)
{
    gd_eval_led_on(LED1);
    vTaskDelay(500);
    gd_eval_led_off(LED1);
    vTaskDelay(500);
}


/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void main(void)
{

    gd_eval_led_init(LED1);
    systick_config();

    xTaskCreate(DemoTask, (char const *)"DemoTask", 256, NULL, tskIDLE_PRIORITY + 2, NULL);

    /* start the RTOS scheduler, this function should not return as it causes the
    execution context to change from main() to one of the created tasks. */
    vTaskStartScheduler();

    /* should never get here! */
    return;
}
