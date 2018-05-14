#include "ioCC2530.h"
#include "intr.h"

unsigned int sys_clk_s = 0;  //ϵͳʱ�� s
unsigned long sys_clk_ms = 0;  //ϵͳʱ�� ms

#pragma vector=URX0_VECTOR
__interrupt void UART0_ISR(void)
{

}

#pragma vector=T1_VECTOR
__interrupt void Timer1_ISR(void)
{
	sys_clk_ms++;
	if(sys_clk_ms >= 1000) 
	{
		sys_clk_ms = 0;
		sys_clk_s++;
	}
}

#pragma vector=RF_VECTOR
__interrupt void rf_isr(void)
{

}