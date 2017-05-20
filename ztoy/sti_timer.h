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

#define            GENERAL_TIM                   TIM3
#define            GENERAL_TIM_APBxClock_FUN     RCC_APB1PeriphClockCmd
#define            GENERAL_TIM_CLK               RCC_APB1Periph_TIM3
#define            GENERAL_TIM_Period            (4000-1)
#define            GENERAL_TIM_Prescaler         (36000-1)
// TIM3 ����Ƚ�ͨ��1
#define            GENERAL_TIM_CH1_GPIO_CLK      RCC_APB2Periph_GPIOA
#define            GENERAL_TIM_CH1_PORT          GPIOA
#define            GENERAL_TIM_CH1_PIN           GPIO_Pin_6

// TIM3 ����Ƚ�ͨ��2
#define            GENERAL_TIM_CH2_GPIO_CLK      RCC_APB2Periph_GPIOA
#define            GENERAL_TIM_CH2_PORT          GPIOA
#define            GENERAL_TIM_CH2_PIN           GPIO_Pin_7

// TIM3 ����Ƚ�ͨ��3
#define            GENERAL_TIM_CH3_GPIO_CLK      RCC_APB2Periph_GPIOB
#define            GENERAL_TIM_CH3_PORT          GPIOB
#define            GENERAL_TIM_CH3_PIN           GPIO_Pin_0

// TIM3 ����Ƚ�ͨ��4
#define            GENERAL_TIM_CH4_GPIO_CLK      RCC_APB2Periph_GPIOB
#define            GENERAL_TIM_CH4_PORT          GPIOB
#define            GENERAL_TIM_CH4_PIN           GPIO_Pin_1

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

void GENERAL_TIM_Init(void);

#endif /* _STI_TIMER_H__ */


