/*
�﷨�涨
1. ���д
2. ������������ĸ��д������֮�����»���
3. ȫ�ֱ�����������ĸ��д������֮��û���»���
4. �ֲ�����Сд
*/

#ifndef __LOS_CONFIG_H__
#define __LOS_CONFIG_H__

#define config_USE_STDLIB       1       /* 1:use stdlib 0:use custom */
#define config_MAX_TASK         10      /* max task num < 256 */
#define config_SYS_CLK          72000000

#define Task_Sys_Clk            SysTick_Handler


#endif  /* __LOS_CONFIG_H__ */