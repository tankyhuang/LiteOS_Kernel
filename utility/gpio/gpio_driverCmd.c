#include <stdio.h>
#include <stm32f10x.h>
#include <console.h>
#include <debug.h>
#include <gpio_conf.h>
#include <sti_gpio.h>
#include <bsp_led.h>


#if DEBUG_ENABLE

//////////////////////////////////////////////////////////////////////////////
//	CONSTANT DEFINITIONS
//////////////////////////////////////////////////////////////////////////////

static void GPIOConsole_Input(char *szArguments);
static void GPIOConsole_Output(char *szArguments);
static void GPIOConsole_PWM(char *szArguments);

static SCONSOLECOMMAND sConsoleGPIO[] =
{
    { "ioin" ,      "<%c><%d>"     ,        "Get GPIO pin"       ,              GPIOConsole_Input   },
    { "iout" ,      "<%c><%d><%d>" ,        "Set GPIO pin"       ,              GPIOConsole_Output  },
	{ "pwm",		"<group> <bit>",		"PWM func", 					    GPIOConsole_PWM	    },
	{ NULL, NULL, NULL, NULL }
};

//////////////////////////////////////////////////////////////////////////////
//	TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
//	VARIABLE DEFINITIONS
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
//	FUNCTION DECLARATIONS
//////////////////////////////////////////////////////////////////////////////

void RegisterGPIODriverCommand(void)
{
    consoleRegisterFunctions(sConsoleGPIO);
}

//////////////////////////////////////////////////////////////////////////////
// usage : iout a 11 1
static void GPIOConsole_Output(char *szArguments)
{
	char param1; // port
	int param2; // pin
	int param3; // 0 or 1
    STI_GPIO_GROUP_TYPE group;
    
	sscanf(szArguments, "%c %d %d", &param1, &param2, &param3);

    // Default 2Mhz, PP mode
    //printf("%c %d %d\n", param1, param2, param3 );
    // Get GPIO GROUP sti_GPIOA ... sti_GPIOG
    group = (STI_GPIO_GROUP_TYPE)(sti_GPIOA + (param1 - 'a'));
    
    sti_ConfigGPIOEx(group, (uint16_t)param2, GPIO_Speed_2MHz, GPIO_Mode_Out_PP);
    sti_SetGPIOEx(group,(uint16_t)param2, (uint8_t)param3);
}

//////////////////////////////////////////////////////////////////////////////

static void GPIOConsole_Input(char *szArguments)
{
	int			param1;	// 
	int			param2;	// 
    uint8_t     value;

	sscanf(szArguments, "%d %d", &param1, &param2);

    value = sti_GetGPIOEx((STI_GPIO_GROUP_TYPE)param1, (uint16_t)param2);
    
    printf("port[%c] %d\n", ('A'+param1), value);
    
	return;
}

static void GPIOConsole_PWM(char *szArguments)
{
	int param1;

	sscanf(szArguments, "%d", &param1);
}

#endif

