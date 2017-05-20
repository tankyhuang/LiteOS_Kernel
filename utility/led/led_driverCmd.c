#include <stdlib.h>
#include <stdio.h>
#include <console.h>
#include <debug.h>
#include <bsp_led.h>
#include <led_driver.h>
#include <led_driverCmd.h>


#if DEBUG_ENABLE

//////////////////////////////////////////////////////////////////////////////
//	CONSTANT DEFINITIONS
//////////////////////////////////////////////////////////////////////////////

static void LEDConsole_OnOff(char *szArguments);
static void LEDConsole_Blink(char *szArguments);
static void LEDConsole_ChangeDuty(char *szArguments);

static SCONSOLECOMMAND sConsoleLed[] =
{
	{ "led",		"<LED Type> <On/Off>",							"On/Off LED",	    LEDConsole_OnOff			},
	{ "ledblink",	"<LED Type> <Frequency> <Duty> <OnOffCount>",	"Blink LED",	    LEDConsole_Blink	        },
	{ "ledduty",	"<LED Type> <Frequency> <Duty> <OnOffCount>",	"Change PWM Duty",	LEDConsole_ChangeDuty	    },
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

void RegisterLEDDriverCommand(void)
{
    consoleRegisterFunctions(sConsoleLed);
}

//////////////////////////////////////////////////////////////////////////////

	static void PrintUsageLed(void)
	{
		printf("Syntax : led <LED Type> <On/Off>\r\n");
		printf("          <LED Type> 0 -> STATUS LED GREEN\r\n");
		printf("                     1 -> STATUS LED RED\r\n");
		printf("                     2 -> POWER LED\r\n");
		printf("          <On/Off>   0 -> Off\r\n");
		printf("                     1 -> On\r\n");
	}

static void LEDConsole_OnOff(char *szArguments)
{
	int param1;
	int param2;

	sscanf(szArguments, "%d %d", &param1, &param2);

	if (param2 == 0)
	{
		switch (param1)
		{
			case 0:		LEDDrv_Off(ledtype_StatusLED_Green);	break;
			case 1:		LEDDrv_Off(ledtype_StatusLED_Red);		break;
			default:	goto usage;

		}
	}
	else if (param2 == 1)
	{
		switch (param1)
		{
			case 0:		LEDDrv_On(ledtype_StatusLED_Green);	break;
			case 1:		LEDDrv_On(ledtype_StatusLED_Red);	break;
			default:	goto usage;
		}
	}
	else
	{
		goto usage;
	}

	return;

usage :
	PrintUsageLed();
}

//////////////////////////////////////////////////////////////////////////////

	static void PrintUsageLedBlink(void)
	{
		printf("Syntax : ledblink <LED Type> <Frequency> <Duty> <OnOffCount>\r\n");
		printf("        <LED Type>   0   -> STATUS LED GRENN\r\n");
		printf("                     1   -> STATUS LED RED\r\n");
		printf("                     2   -> POWER LED\r\n");

		printf("        <Frequency>  (0 < num <= 10000)\r\n");
		printf("                     num[unit]=[Hz * 1000]\r\n");
		printf("        <Duty>       0   -> 0%\r\n");
		printf("                     20  -> 20%\r\n");
		printf("                     25  -> 25%\r\n");
		printf("                     50  -> 50%\r\n");
		printf("                     75  -> 75%\r\n");
		printf("                     100 -> 100%\r\n");
		printf("        <OnOffCount> Count Num(one count in On & Off)\r\n");
	}

// ledblink 0 2000 25 10
static void LEDConsole_Blink(char *szArguments)
{
	int			param1;	// LED Type
	int			param2;	// Frequency of Blink 1
	int			param3;	// Duty (200 : Secret command)
	int			param4;	// Count Number(one count in On & Off)
	LED_TYPE	type;	// LED Type
	uint16_t		freq;	// Frequency of Blink
	LEDBL_DUTY	duty;	// Duty

	sscanf(szArguments, "%d %d %d %d", &param1, &param2, &param3, &param4);

	switch (param1)
	{
		case 0 :	type = ledtype_StatusLED_Green;	break;
		case 1 :	type = ledtype_StatusLED_Red;	break;
		default :	goto usage;
	}

	if( (param2 == 0) || (param2 > 10000) ){ goto usage; }
	freq = (uint16_t)param2;

	switch (param3)
	{
		case 0 :	duty = ledblduty_0;		break;
		case 20 :	duty = ledblduty_20;	break;
		case 25 :	duty = ledblduty_25;	break;
		case 50 :	duty = ledblduty_50;	break;
		case 75 :	duty = ledblduty_75;	break;
		case 100 :	duty = ledblduty_100;	break;
		case 200 :	duty = (LEDBL_DUTY)200;				break;		// Secret command
		default :	goto usage;
	}

	if (param3 == 200)
	{
		LEDDrv_Blink(type, freq);
	}
	else
	{
		LEDDrv_BlinkDutyWithCount(type, freq, duty, param4);
	}

	return;

usage :
	PrintUsageLedBlink();
}

static void LEDConsole_ChangeDuty(char *szArguments)
{
	int param1;

	sscanf(szArguments, "%d", &param1);

	LEDDrv_ChangePwmDuty(param1);
}

#endif

