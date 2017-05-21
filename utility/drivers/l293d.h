#ifndef __L293D_H_
#define __L293D_H_

#include "cmsis_os.h"
#include "gpio_conf.h"


#ifdef __cplusplus
extern "C"
{
#endif


#define L293D_IN1_ENABLE   GPIO_SetBits(GPIO_L293D_IN1_PORT, GPIO_L293D_IN1_PIN)
#define L293D_IN2_ENABLE   GPIO_SetBits(GPIO_L293D_IN2_PORT, GPIO_L293D_IN2_PIN)
#define L293D_IN3_ENABLE   GPIO_SetBits(GPIO_L293D_IN3_PORT, GPIO_L293D_IN3_PIN)
#define L293D_IN4_ENABLE   GPIO_SetBits(GPIO_L293D_IN4_PORT, GPIO_L293D_IN4_PIN)

#define L293D_IN1_DISABLE  GPIO_ResetBits(GPIO_L293D_IN1_PORT, GPIO_L293D_IN1_PIN)
#define L293D_IN2_DISABLE  GPIO_ResetBits(GPIO_L293D_IN2_PORT, GPIO_L293D_IN2_PIN)
#define L293D_IN3_DISABLE  GPIO_ResetBits(GPIO_L293D_IN3_PORT, GPIO_L293D_IN3_PIN)
#define L293D_IN4_DISABLE  GPIO_ResetBits(GPIO_L293D_IN4_PORT, GPIO_L293D_IN4_PIN)


#define L293D_INx_DISABLE_ALL  L293D_IN1_DISABLE; \
                               L293D_IN2_DISABLE; \
                               L293D_IN3_DISABLE; \
                               L293D_IN4_DISABLE    
//////////////////////////////////////////////////////////////////////////////
//	CONSTANT DEFINITIONS
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
//	TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////
//	FUNCTION DECLARATIONS
//////////////////////////////////////////////////////////////////////////////



void l293dInit(void);
void l293dDeInit(void);

#ifdef __cplusplus
}
#endif

#endif  // __L293D_H_
