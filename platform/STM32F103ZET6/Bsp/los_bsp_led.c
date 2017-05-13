#include "stdlib.h"
#include "stdio.h"
#include "los_bsp_led.h"
#include "bsp_led.h"
#include "gpio.h"

/******************************************************************************
    here include some special hearder file you need
******************************************************************************/

 /**
  * @brief  ��ʼ������LED��IO
  * @param  ��
  * @retval ��
  */
void LED_GPIO_Config(void)
{		
    /*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /*����LED��ص�GPIO����ʱ��*/
    RCC_APB2PeriphClockCmd( GPIO_LED_STATUS_GREEN_CLK, ENABLE);
    /*ѡ��Ҫ���Ƶ�GPIO����*/
    GPIO_InitStructure.GPIO_Pin = GPIO_LED_STATUS_GREEN_BIT;    
    
    /*��������ģʽΪͨ���������*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
    
    /*������������Ϊ50MHz */   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    
    /*���ÿ⺯������ʼ��GPIO*/
    GPIO_Init(GPIO_LED_STATUS_GREEN_GROUP, &GPIO_InitStructure);    
    
    /* �ر�����led��    */
    GPIO_SetBits(GPIO_LED_STATUS_GREEN_GROUP, GPIO_LED_STATUS_GREEN_BIT);
}

/*********************************************END OF FILE**********************/


/*****************************************************************************
 Function    : LOS_EvbLedInit
 Description : Init LED device
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
void LOS_EvbLedInit(void)
{
    //add you code here.
    LED_GPIO_Config();
    return ;
}

/*************************************************************************************************
 *  Function    ��LOS_EvbLedControl
 *  Discription : Led control function
 *  Input       : (1) index Led's index                                                                  *
 *                (2) cmd   Led on or off  
 *  Output      : None
 *  Return      : None                                                                                *
 *                                                                                 *
**************************************************************************************************/
void LOS_EvbLedControl(int index, int cmd)
{
    switch (index)
    {
        case LOS_LED1:
        {
            if (cmd == LED_ON)
            {
                //add you code here.
                        /*led1 on */
            }
            else
            {
                //add you code here.
                        /*led1 off */
            }
            break;
        }
        case LOS_LED2:
        {
            if (cmd == LED_ON)
            {
            //printf("Led on\r\n");
							//LED1_ON;
                //add you code here.
                        /*led2 on */
            }
            else
            {
                //printf("Led off\r\n");
							//LED1_OFF;
                //add you code here.
                        /*led2 off */
            }
            break;
        }
        case LOS_LED3:
        {
            if (cmd == LED_ON)
            {
                //add you code here.
                        /*led3 on */
            }
            else
            {
                //add you code here.
                        /*led3 off */
            }
            break;
        }
        default:
        {
            break;
        }
    }
    return;
}



