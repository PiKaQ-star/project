#include "stm32f10x.h"
#include "delay.h"

void delay_init(void)
{
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
}

void delay_ms(uint32_t ms)
{
    uint32_t i,j;
    for(i=0;i<ms;i++)
        for(j=0;j<7200;j++);
}
