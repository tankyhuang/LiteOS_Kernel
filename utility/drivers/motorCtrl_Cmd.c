#include <stdlib.h>
#include <stdio.h>
#include <stm32f10x.h>
#include <console.h>
#include <debug.h>
#include <l293d.h>
#include <motorctrl.h>


#if DEBUG_ENABLE

//////////////////////////////////////////////////////////////////////////////
//	CONSTANT DEFINITIONS
//////////////////////////////////////////////////////////////////////////////

static void MotorConsole_l293d(char *szArguments);
static void MotorConsole_Rotate(char *szArguments);

static SCONSOLECOMMAND sConsoleMotorCtrl[] =
{
	{ "l293d",		"<INx <On/Off>",							"On/Off INx",	    MotorConsole_l293d			},
	{ "motor",	    "<Motor Type> <Frequency> <Duty> <OnOffCount>",	"Motor",	    MotorConsole_Rotate	        },
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

void RegisterMotorDrvCommand(void)
{
    consoleRegisterFunctions(sConsoleMotorCtrl);
}

//////////////////////////////////////////////////////////////////////////////

static void PrintUsageMotor(void)
{
}

static void MotorConsole_l293d(char *szArguments)
{
	int param1;
	int param2;

	sscanf(szArguments, "%d %d", &param1, &param2);

    if ( param1 > 4 )
    {
        goto usage;
    }
	switch (param1)
    {
        case 1:
            param2 ? L293D_IN1_ENABLE : L293D_IN1_DISABLE;
            break;
        case 2:
            param2 ? L293D_IN2_ENABLE : L293D_IN2_DISABLE;
            break;
        case 3:
            param2 ? L293D_IN3_ENABLE : L293D_IN3_DISABLE;
            break;
        case 4:
            param2 ? L293D_IN4_ENABLE : L293D_IN4_DISABLE;
            break;
        default:
            break;
    }
	return;

usage :
	PrintUsageMotor();
}

//////////////////////////////////////////////////////////////////////////////


static void MotorConsole_Rotate(char *szArguments)
{
	int			param1;	//  Type
	int			param2;	// Frequency of Blink
	
	sscanf(szArguments, "%d %d", &param1, &param2);

    if ( param1 >= MOTOR_TYPE_COUNT || param2 >= ROTATE_TYPE_COUNT )
    {
        MOTORCtrl_StopAll();
        goto usage;
    }
    
    MOTORCtrl_StopAll();
    MOTORCtrl_Rotate((MOTOR_TYPE)param1, (ROTATE_TYPE)param2);
	return;

usage :
	PrintUsageMotor();
}


#endif

