#include <cmsis_os.h>
#include <los_typedef.h>
#include <los_swtmr.h>
#include <bsp_led.h>
#include <debug.h>
#include <gpio_conf.h>
#include "gpio_driverCmd.h"
#include "motor.h"
#include "interface.h"
//#include "motorctrl.h"
//#include "motorctrl_cmd.h"
//#include "l293d.h"

void initializeGPIO()
{

#if DEBUG_ENABLE
    RegisterGPIODriverCommand();
    //RegisterMotorDrvCommand();
#endif    
    //GPIOCLKInit();
    //TIM2_Init();
    //MotorInit();
    //l293dInit();
    //ygGoForward();
    //ygGoBackward();
    //ygTurnLeft();
    //ygTurnRight();
    
}

