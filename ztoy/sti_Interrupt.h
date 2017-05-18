/* ****************************************************************** */
/*                             sti_GPIO.h                             */
/* ****************************************************************** */


#ifndef _STI_INTERRUPT_H__
#define _STI_INTERRUPT_H__

//  ============================================================================================================================
//    NVIC_PriorityGroup   | NVIC_IRQChannelPreemptionPriority | NVIC_IRQChannelSubPriority  | Description
//  ============================================================================================================================
//   NVIC_PriorityGroup_0  |                0                  |            0-15             |   0 bits for 
//                         |                                   |                             |   4 bits for subpriority
//  ----------------------------------------------------------------------------------------------------------------------------
//   NVIC_PriorityGroup_1  |                0-1                |            0-7              |   1 bits for pre-emption priority
//                         |                                   |                             |   3 bits for subpriority
//  ----------------------------------------------------------------------------------------------------------------------------    
//   NVIC_PriorityGroup_2  |                0-3                |            0-3              |   2 bits for pre-emption priority
//                         |                                   |                             |   2 bits for subpriority
//  ----------------------------------------------------------------------------------------------------------------------------    
//   NVIC_PriorityGroup_3  |                0-7                |            0-1              |   3 bits for pre-emption priority
//                         |                                   |                             |   1 bits for subpriority
//  ----------------------------------------------------------------------------------------------------------------------------    
//   NVIC_PriorityGroup_4  |                0-15               |            0                |   4 bits for pre-emption priority
//                         |                                   |                             |   0 bits for subpriority                       
//  ============================================================================================================================

typedef enum
{
    sti_IRQ_ID_WWDG                 = 0,
    sti_IRQ_ID_PVD                     ,
    sti_IRQ_ID_TAMPER                  ,
    sti_IRQ_ID_RTC                     ,
    sti_IRQ_ID_FLASH                   ,
    sti_IRQ_ID_RCC                     ,
    sti_IRQ_ID_EXTI0                   ,
    sti_IRQ_ID_EXTI1                   ,
    sti_IRQ_ID_EXTI2                   ,
    sti_IRQ_ID_EXTI3                   ,
    sti_IRQ_ID_EXTI4                   ,
    sti_IRQ_ID_DMA1_Channel1           ,
    sti_IRQ_ID_DMA1_Channel2           ,
    sti_IRQ_ID_DMA1_Channel3           ,
    sti_IRQ_ID_DMA1_Channel4           ,
    sti_IRQ_ID_DMA1_Channel5           ,
    sti_IRQ_ID_DMA1_Channel6           ,
    sti_IRQ_ID_DMA1_Channel7           ,
    sti_IRQ_ID_ADC1_2                  ,
    sti_IRQ_ID_USB_HP_CAN1_TX          ,
    sti_IRQ_ID_USB_LP_CAN1_RX0         ,
    sti_IRQ_ID_CAN1_RX1                ,
    sti_IRQ_ID_CAN1_SCE                ,
    sti_IRQ_ID_EXTI9_5                 ,
    sti_IRQ_ID_TIM1_BRK                ,
    sti_IRQ_ID_TIM1_UP                 ,
    sti_IRQ_ID_TIM1_TRG_COM            ,
    sti_IRQ_ID_TIM1_CC                 ,
    sti_IRQ_ID_TIM2                    ,
    sti_IRQ_ID_TIM3                    ,
    sti_IRQ_ID_TIM4                    ,
    sti_IRQ_ID_I2C1_EV                 ,
    sti_IRQ_ID_I2C1_ER                 ,
    sti_IRQ_ID_I2C2_EV                 ,
    sti_IRQ_ID_I2C2_ER                 ,
    sti_IRQ_ID_SPI1                    ,
    sti_IRQ_ID_SPI2                    ,
    sti_IRQ_ID_USART1                  ,
    sti_IRQ_ID_USART2                  ,
    sti_IRQ_ID_USART3                  ,
    sti_IRQ_ID_EXTI15_10               ,
    sti_IRQ_ID_RTCAlarm                ,
    sti_IRQ_ID_USBWakeUp               ,
    sti_IRQ_ID_TIM8_BRK                ,
    sti_IRQ_ID_TIM8_UP                 ,
    sti_IRQ_ID_TIM8_TRG_COM            ,
    sti_IRQ_ID_TIM8_CC                 ,
    sti_IRQ_ID_ADC3                    ,
    sti_IRQ_ID_FSMC                    ,
    sti_IRQ_ID_SDIO                    ,
    sti_IRQ_ID_TIM5                    ,
    sti_IRQ_ID_SPI3                    ,
    sti_IRQ_ID_UART4                   ,
    sti_IRQ_ID_UART5                   ,
    sti_IRQ_ID_TIM6                    ,
    sti_IRQ_ID_TIM7                    ,
    sti_IRQ_ID_DMA2_Channel1           ,
    sti_IRQ_ID_DMA2_Channel2           ,
    sti_IRQ_ID_DMA2_Channel3           ,
    sti_IRQ_ID_DMA2_Channel4_5         ,
    
    sti_IRQ_ID_NUM
}STI_IRQ_ID_TYPE;

typedef void (*P_STI_IRQ_HANDLER)( void );

#define STI_HW_IRQ_HANDLER_Register(id, handler, priority)             \
        {                                                              \
            uint32_t ret;                                               \
            ret = LOS_HwiCreate(  id,                                   \
                                priority,                               \
                                0,                                      \
                                handler,                                \
                                0);                                     \
                                                                        \
            ASSERT(LOS_OK == ret);                                      \
        }

#define STI_HW_IRQ_HANDLER_UnRegister(id, handler, priority)           \
        {                                                              \
            uint32_t ret = LOS_HwiDelete(id);                          \
            ASSERT(LOS_OK == ret);                                     \
        }


void
sti_ResgiterIRQHandler( P_STI_IRQ_HANDLER isr , 
                        STI_IRQ_ID_TYPE id,
                        uint32_t priority);

void 
sti_UnResgiterIRQHandler( STI_IRQ_ID_TYPE id );

#endif /* _STI_INTERRUPT_H__ */


