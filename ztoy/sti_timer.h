/* ****************************************************************** */
/*                             sti_GPIO.h                             */
/* ****************************************************************** */


#ifndef _STI_TIMER_H__
#define _STI_TIMER_H__

#include "stm32f10x.h"

//////////////////////////////////////////////////////////////////////////////
//	CONSTIANT DEFINITIONS
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
//	TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////////


typedef enum
{
    sti_TimerId_Invalid               = 0,
    sti_TimerId_Advanced1            ,    
    sti_TimerId_Genera2              ,
    sti_TimerId_Genera3              ,
    sti_TimerId_Genera4              ,
    sti_TimerId_Genera5              ,
    sti_TimerId_Basic6               ,
    sti_TimerId_Basic7               ,
    sti_TimerId_Advance8             ,
    
    sti_TimerId_Num
}STI_TIMER_ID_TYPE;



typedef void (*STI_TIMER_HANDLE)(void);

// cycle = 1/GET_SYS_FREQ_HZ() * ( prescaler + 1) * ( arr + 1)
// period = cycle * GET_SYS_FREQ_HZ()/ (prescaler + 1) - 1;
// if cycle = 1000ms
// period = 1000 * (72000000 / 36000) - 1 = 1999


#define STI_HW_TIMER_Create(TIMx, prescaler, period, mode)              \
        {                                                               \
            TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;             \
            TIM_TimeBaseStructure.TIM_Period        = period - 1;       \
            TIM_TimeBaseStructure.TIM_Prescaler     = prescaler - 1;    \
            TIM_TimeBaseStructure.TIM_ClockDivision = 0;                \
            TIM_TimeBaseStructure.TIM_CounterMode   = mode;             \
            TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);             \
        }
        
#define STI_NVIC_Config(preempt_priority, sub_priority, IRQ_ch, cmd)    \
        {                                                               \
            NVIC_InitTypeDef NVIC_InitStructure;                        \
            NVIC_InitStructure.NVIC_IRQChannel = IRQ_ch;                \
            NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = preempt_priority;   \
            NVIC_InitStructure.NVIC_IRQChannelSubPriority = sub_priority;              \
            NVIC_InitStructure.NVIC_IRQChannelCmd = cmd;                \
            NVIC_Init(&NVIC_InitStructure);                             \
        }

#define STI_HW_TIMER_Start(TIMx)                                        \
        {                                                               \
            TIM_ClearITPendingBit(TIMx, TIM_IT_Update);                 \
            TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);                  \
            TIM_Cmd(TIMx, ENABLE);                                      \
        }

#define STI_HW_TIMER_Stop(TIMx)                                        \
        {                                                              \
            TIM_ClearITPendingBit(TIMx, TIM_IT_Update);                \
            TIM_ITConfig(TIMx, TIM_IT_Update, DISABLE);                \
            TIM_Cmd(TIMx, DISABLE);                                    \
        }

      
//////////////////////////////////////////////////////////////////////////////
//	FUNCTION DECLARATIONS
//////////////////////////////////////////////////////////////////////////////
void TIM2_Init(void);

#endif /* _STI_TIMER_H__ */


