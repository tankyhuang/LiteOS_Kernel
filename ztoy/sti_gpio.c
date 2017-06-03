/* ****************************************************************** */
/*                                GPIO.c                              */
/*                                                                    */
/*                    Application Level GPIO Functions                */
/* ****************************************************************** */
#include "stm32f10x.h"
#include "types.h"
#include "sti_GPIO.h"


void
sti_ConfigGPIOEx( STI_GPIO_GROUP_TYPE group,
                  uint16_t pin, 
                  GPIOSpeed_TypeDef speed,
                  GPIOMode_TypeDef mode )
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_TypeDef *psGPIOx;

    assert_param( group < sti_GPIO_Num );
   
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA<<group, ENABLE);
   
    psGPIOx = (GPIO_TypeDef *)(GPIOA_BASE + 0x0400 * group);
    
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_0<<pin;        
    GPIO_InitStructure.GPIO_Mode    = mode;    
    GPIO_InitStructure.GPIO_Speed   = speed;
    
    GPIO_Init(psGPIOx, &GPIO_InitStructure);
}


void 
sti_SetGPIOEx( STI_GPIO_GROUP_TYPE group, 
               uint16_t pin, 
               uint8_t logic )
{
    GPIO_TypeDef *psGPIOx;
   
    psGPIOx = (GPIO_TypeDef *)(GPIOA_BASE + 0x0400 * group);
    
    if ( logic )
    {
        GPIO_SetBits(psGPIOx, GPIO_Pin_0<<pin);
    }
    else
    {
        GPIO_ResetBits(psGPIOx, GPIO_Pin_0<<pin);
    }
}


uint8_t
sti_GetGPIOEx( STI_GPIO_GROUP_TYPE group,
               uint16_t pin )
{
    GPIO_TypeDef *psGPIOx;
   
    psGPIOx = (GPIO_TypeDef *)(GPIOA_BASE + 0x0400 * group);
    
    return GPIO_ReadInputDataBit(psGPIOx, GPIO_Pin_0<<pin);
}

