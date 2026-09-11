#include "stm32f10x.h"
#include "blub.h"
#include "beep.h"
#include "sg90.h"

static void bulb_send_byte(uint8_t ch)
{
    USART_SendData(USART1, ch);
    while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
}

static void rgb_led_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOB, GPIO_Pin_0 | GPIO_Pin_1);
    GPIO_ResetBits(GPIOA, GPIO_Pin_3);
}

void bulb_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    USART_InitStruct.USART_BaudRate = 115200;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStruct);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
    USART_Cmd(USART1, ENABLE);
    rgb_led_init();
}

void bulb_send_str(char *str)
{
    while(*str)
    {
        USART_SendData(USART1, *str++);
        while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
    }
}

void USART1_IRQHandler(void)
{
    uint8_t ch;
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        ch = USART_ReceiveData(USART1);
        bulb_send_byte(ch);
        if(ch == 'R')
        {
            GPIO_SetBits(GPIOB,GPIO_Pin_1);
            GPIO_ResetBits(GPIOB,GPIO_Pin_0);
            GPIO_ResetBits(GPIOA,GPIO_Pin_3);
        }
        else if(ch == 'G')
        {
            GPIO_ResetBits(GPIOB,GPIO_Pin_1);
            GPIO_SetBits(GPIOB,GPIO_Pin_0);
            GPIO_ResetBits(GPIOA,GPIO_Pin_3);
        }
        else if(ch == 'B')
        {
            GPIO_ResetBits(GPIOB,GPIO_Pin_0);
            GPIO_ResetBits(GPIOB,GPIO_Pin_1);
            GPIO_SetBits(GPIOA,GPIO_Pin_3);
        }
        else if(ch == 'Y')
        {
            GPIO_SetBits(GPIOB,GPIO_Pin_1);
            GPIO_SetBits(GPIOB,GPIO_Pin_0);
            GPIO_ResetBits(GPIOA,GPIO_Pin_3);
        }
        else if(ch == 'S')
        {
            beep_on();
            beep_set_force(1);
        }
        else if(ch == 'O')
        {
            beep_off();
            beep_set_force(1);
        }
        else if(ch == 'U')
        {
            beep_set_force(0);
        }
        else if(ch == 'Z')
        {
            beep_off();
            sg90_set_angle(0);
            beep_set_force(1);
        }
    }
}
