/* ****************************************************************** */
/*                                sti_Timer.c                          */
/*                                                                    */
/*                    Application Level GPIO Functions                */
/* ****************************************************************** */
#include "stm32f10x.h"                  // Device header
#include <types.h>
#include <debug.h>
#include <los_hwi.h>
#include <freq.h>
#include <sti_gpio.h>
#include <sti_timer.h>
#include <sti_interrupt.h>
    

static int count = 0;

// cycle = 1/GET_SYS_FREQ_HZ() * ( prescaler + 1) * ( arr + 1)
// period = cycle * GET_SYS_FREQ_HZ()/ (prescaler + 1) - 1;
// if cycle = 1000ms
// period = 1000 * (72000000 / 36000) - 1 = 1999
void TIMtest_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
        if ( count++ % 2 )
        {
            GPIO_SetBits(GPIOB, GPIO_Pin_12);
        }
        else
        {
            GPIO_ResetBits(GPIOB, GPIO_Pin_12);
        }
    }
}

void TIM2_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    // 2000/(72000000/36000 ) = 1s
    STI_HW_TIMER_Create(TIM2, 36000, 2000, TIM_CounterMode_Up );
    STI_HW_IRQ_HANDLER_Register(sti_IRQ_ID_TIM2, TIMtest_IRQHandler, OS_HWI_PRIO_HIGHEST );
    
    STI_NVIC_Config( 0, 1, TIM2_IRQn, ENABLE);
    STI_HW_TIMER_Start( TIM2);
}



