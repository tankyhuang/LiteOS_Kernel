/* ****************************************************************** */
/*                             sti_GPIO.h                             */
/* ****************************************************************** */


#ifndef _STI_GPIO_H__
#define _STI_GPIO_H__

#include "stm32f10x.h"

//////////////////////////////////////////////////////////////////////////////
//	CONSTIANT DEFINITIONS
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
//	TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////////

typedef enum {
    sti_GPIO_Out             = 0x00,
    sti_GPIO_In              = 0x01
} STI_GPIO_CONFIG_TYPE;



typedef enum {
    sti_GPIO_Func             = 0x00,
    sti_GPIO_Gpio             = 0x40,
} STI_GPIO_FUNC_TYPE;



typedef enum {
    sti_GPIOA                       = 0x00,
    sti_GPIOB                       = 0x01,
    sti_GPIOC                       = 0x02,
    sti_GPIOD                       = 0x03,
    sti_GPIOE                       = 0x04,
    sti_GPIOF                       = 0x05,
    sti_GPIOG                       = 0x06,
    
    sti_GPIO_Num
}STI_GPIO_GROUP_TYPE;





//////////////////////////////////////////////////////////////////////////////
//	FUNCTION DECLARATIONS
//////////////////////////////////////////////////////////////////////////////


void
sti_ConfigGPIOEx( STI_GPIO_GROUP_TYPE group, 
                  uint16_t pin, 
                  GPIOSpeed_TypeDef speed,
                  GPIOMode_TypeDef mode );


void 
sti_SetGPIOEx( STI_GPIO_GROUP_TYPE group, 
               uint16_t pin, 
               uint8_t logic );

uint8_t
sti_GetGPIOEx( STI_GPIO_GROUP_TYPE group, 
               uint16_t pin); 
      


#endif /* _STI_GPIO_H__ */


