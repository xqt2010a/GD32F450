/*
语法规定
1. 宏大写
2. 函数单词首字母大写，单词之间用下划线
3. 全局变量单词首字母大写，单词之间没有下划线
4. 局部变量小写
*/

#ifndef __LOS_CONFIG_H__
#define __LOS_CONFIG_H__

#define config_USE_STDLIB       1       /* 1:use stdlib 0:use custom */
#define config_MAX_TASK         10      /* max task num < 256 */
#define config_SYS_CLK          72000000

#define Task_Sys_Clk            SysTick_Handler


#endif  /* __LOS_CONFIG_H__ */