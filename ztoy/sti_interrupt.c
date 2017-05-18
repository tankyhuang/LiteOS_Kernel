/* ****************************************************************** */
/*                                sti_Interrupt.c                          */
/*                                                                    */
/*                    Application Level GPIO Functions                */
/* ****************************************************************** */
#include "stm32f10x.h"
#include "los_hwi.h"
#include "sti_interrupt.h"
#include "debug.h"

void
sti_ResgiterIRQHandler( P_STI_IRQ_HANDLER isr , 
                        STI_IRQ_ID_TYPE id,
                        uint32_t priority)
{
    uint32_t ret;
    ret = LOS_HwiCreate(  id,
                          priority,
                          0,
                          isr,
                          0);

    ASSERT(LOS_OK == ret);                          
}

void 
sti_UnResgiterIRQHandler( STI_IRQ_ID_TYPE id )
{
    uint32_t ret = LOS_HwiDelete(id);
    
    ASSERT(LOS_OK == ret);
}


