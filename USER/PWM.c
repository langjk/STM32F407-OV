#include "sys.h"
#include "stdio.h"
#include "PWM.h"
void TIM14_PWM_Init(u32 arr,u32 psc){                              

GPIO_InitTypeDef            GPIO_InitStructure;
TIM_TimeBaseInitTypeDef        TIM_TimeBaseStructure;
TIM_OCInitTypeDef            TIM_OCInitStructure;

RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14,ENABLE);      
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);   

GPIO_PinAFConfig(GPIOF,GPIO_PinSource9,GPIO_AF_TIM14); 

GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;      
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;   
GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
GPIO_Init(GPIOF,&GPIO_InitStructure);          

TIM_TimeBaseStructure.TIM_Prescaler=psc;  
TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
TIM_TimeBaseStructure.TIM_Period=arr;   
TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
TIM_TimeBaseInit(TIM14,&TIM_TimeBaseStructure);

TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
TIM_OC1Init(TIM14, &TIM_OCInitStructure); 

TIM_OC1PreloadConfig(TIM14, TIM_OCPreload_Enable);  
TIM_ARRPreloadConfig(TIM14,ENABLE);
TIM_Cmd(TIM14, ENABLE);                                     
}
