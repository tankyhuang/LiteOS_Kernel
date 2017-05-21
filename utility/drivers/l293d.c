#include <stm32f10x.h>
#include <cmsis_os.h>
#include <los_typedef.h>
#include <l293d.h>


void l293dInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(GPIO_L293D_CLK_ALL, ENABLE);
                            
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;   
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_2MHz; 
    GPIO_InitStructure.GPIO_Pin     = GPIO_L293D_IN1_PIN;   
    GPIO_Init(GPIO_L293D_IN1_PORT,      &GPIO_InitStructure);   
    GPIO_ResetBits(GPIO_L293D_IN1_PORT,  GPIO_L293D_IN1_PIN);
    
    GPIO_InitStructure.GPIO_Pin     = GPIO_L293D_IN2_PIN;   
    GPIO_Init(GPIO_L293D_IN2_PORT,      &GPIO_InitStructure);   
    GPIO_ResetBits(GPIO_L293D_IN3_PORT,  GPIO_L293D_IN2_PIN);
    
    GPIO_InitStructure.GPIO_Pin     = GPIO_L293D_IN3_PIN;   
    GPIO_Init(GPIO_L293D_IN3_PORT,      &GPIO_InitStructure);   
    GPIO_ResetBits(GPIO_L293D_IN3_PORT,  GPIO_L293D_IN3_PIN);
    
    GPIO_InitStructure.GPIO_Pin     = GPIO_L293D_IN4_PIN;   
    GPIO_Init(GPIO_L293D_IN4_PORT,      &GPIO_InitStructure);   
    GPIO_ResetBits(GPIO_L293D_IN4_PORT,  GPIO_L293D_IN4_PIN);
    
    // TODO:ENA ENB pwm control
    GPIO_InitStructure.GPIO_Pin     = GPIO_L293D_ENA_PIN;   
    GPIO_Init(GPIO_L293D_ENA_PORT,      &GPIO_InitStructure);   
    GPIO_ResetBits(GPIO_L293D_ENA_PORT,  GPIO_L293D_ENA_PIN);
    
    GPIO_InitStructure.GPIO_Pin     = GPIO_L293D_ENA_PIN;   
    GPIO_Init(GPIO_L293D_ENB_PORT,      &GPIO_InitStructure);   
    GPIO_ResetBits(GPIO_L293D_ENB_PORT,  GPIO_L293D_ENA_PIN);
}


void l293dDeInit(void)
{
    
}

