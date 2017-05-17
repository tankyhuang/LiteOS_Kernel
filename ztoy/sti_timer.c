/* ****************************************************************** */
/*                                sti_Timer.c                          */
/*                                                                    */
/*                    Application Level GPIO Functions                */
/* ****************************************************************** */
#include "stm32f10x.h"                  // Device header
#include <types.h>
#include "los_hwi.h"
#include <debug.h>
#include <sti_gpio.h>
#include <sti_timer.h>
 
    
static void NVIC_TIM2Configuration(void)
{ 
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable the TIM5 gloabal Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);
}

static VOID User_IRQTIM2Handler(void)
{
    TRACE("\n User IRQ test\n"); 
	  //LOS_InspectStatusSetByID(LOS_INSPECT_INTERRUPT,LOS_INSPECT_STU_SUCCESS);
	  return;
}

void TIM2_Init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    /* TIM2 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period = (4000 - 1);    // 1000ms
    TIM_TimeBaseStructure.TIM_Prescaler = (36000 - 1);//2KHz
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    
    /* TIM2 enable counter */
    TIM_Cmd(TIM2, ENABLE);
    NVIC_TIM2Configuration();
    
    UINTPTR uvIntSave;
    //uvIntSave = LOS_IntLock();   
  
    //LOS_HwiCreate(28, 0, 0, User_IRQTIM2Handler,0);
    
    //LOS_IntRestore(uvIntSave);    
}

static int count = 0;

void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
        if ( count++ % 2 )
        {
            sti_SetGPIOEx(sti_GPIOB, 12, 1);
        }
        else
        {
            sti_SetGPIOEx(sti_GPIOB, 12, 0);
        }
    }
}


