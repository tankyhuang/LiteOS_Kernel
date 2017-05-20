#include "stdlib.h"
#include "stdio.h"
#include "los_bsp_led.h"


/******************************************************************************
    here include some special hearder file you need
******************************************************************************/

 /**
  * @brief  初始化控制LED的IO
  * @param  无
  * @retval 无
  */
void LED_GPIO_Config(void)
{		

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
    return ;
}

/*************************************************************************************************
 *  Function    ：LOS_EvbLedControl
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
             
            }
            else
            {

            }
            break;
        }
        case LOS_LED2:
        {
            if (cmd == LED_ON)
            {
 
            }
            else
            {
            }
            break;
        }
        case LOS_LED3:
        {
            if (cmd == LED_ON)
            {
            }
            else
            {
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



