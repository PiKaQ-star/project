#include "stm32f10x.h"
#include "beep.h"
#include "sg90.h"

static uint8_t force_ctrl = 0;

void beep_init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Prescaler = 72 -1;
    TIM_TimeBaseStructure.TIM_Period = 1000 - 1;
    TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);
    
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC2Init(TIM3,&TIM_OCInitStructure);
    
    TIM_Cmd(TIM3,ENABLE);
    TIM_OC2PreloadConfig(TIM3,TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM3,ENABLE);
}

void beep_on(void)
{
    uint32_t arr = 1000000 / 1000;
    TIM_SetAutoreload(TIM3, arr);
    TIM_SetCompare2(TIM3, arr / 2);
    TIM_GenerateEvent(TIM3, TIM_EventSource_Update);
}

void beep_off(void)
{
    TIM_SetCompare2(TIM3, 0);
    TIM_GenerateEvent(TIM3, TIM_EventSource_Update);
}

void beep_set_force(uint8_t en)
{
    force_ctrl = en;
}

uint8_t get_force_state(void)
{
    return force_ctrl;
}


void light_servo_beep_ctrl(uint16_t val, uint16_t TH, uint8_t *p_run)
{
    static uint16_t filter_cnt = 0;
    static uint8_t last_state = 0;
    *p_run = 0;

    if(force_ctrl == 1)
    {
        beep_off();
        sg90_set_angle(0);
        *p_run = 0;
        return;
    }

   
    uint8_t current_state = (val < TH) ? 1 : 0;

    if(current_state == last_state)
    {
        filter_cnt = 0;
    }
    else
    {
        filter_cnt++;
        if(filter_cnt > 8)
        {
            last_state = current_state;
            filter_cnt = 0;
        }
    }

    if(last_state == 1)
    {
        beep_on();
        *p_run = 1;
    }
    else
    {
        beep_off();
        sg90_set_angle(0);
        *p_run = 0;
    }
}
