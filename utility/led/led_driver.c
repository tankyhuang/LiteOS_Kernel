#include <cmsis_os.h>
#include <los_typedef.h>
#include <los_swtmr.h>
#include <bsp_led.h>
#include <led_driver.h>
#include <led_driverCmd.h>
#include <debug.h>

#define GPIO_LED_STATUS_GREEN_GROUP             GPIOB
#define GPIO_LED_STATUS_GREEN_BIT               GPIO_Pin_12
#define GPIO_LED_STATUS_GREEN_FUNC              I43_GPIO_CONFIG_OUT
#define GPIO_LED_STATUS_GREEN_LOGIC             1

typedef enum {
    I43_GPIO_CONFIG_OUT             = 0x00,
    I43_GPIO_CONFIG_IN              = 0x80
} I43_GPIO_CONFIG_TYPE;

typedef enum {
    I43_XLGPIO                      = 0x00,
    I43_FGPIO                       = 0x01,
    I43_CCDGPIO                     = 0x02,
    I43_DVGPIO                      = 0x03,
    I43_PWMGPIO                     = 0x04,
}I43_GPIO_GROUP_TYPE;

//////////////////////////////////////////////////////////////////////////////
//	CONSTANT DEFINITIONS
//////////////////////////////////////////////////////////////////////////////

enum
{
	LED_BLINK_TIME = 10,
	LED_PWM_FREQ   = 50 * 1000,	// 50KHz
};


//////////////////////////////////////////////////////////////////////////////
//	TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////////

typedef struct LED_STATUS
{
	BOOL		    IsOn;
	BOOL		    IsLocked;
	uint16_t		Millisec;
	uint16_t		CurrentTime;
	uint16_t		MillisecOn;
	uint16_t		MillisecOff;
	int16_t		    OnOffCount;
	int16_t		    OnOffCountMax;
} LED_STATUS;

typedef struct LED_BLINK_ALARM
{
	LED_TYPE	LedType;
	const char	*pName;
} LED_BLINK_ALARM;

typedef struct LedBlinkNotify_t
{
	PLEDHANDLER p_cbr;
	void* p_user_instance;
}LedBlinkNotify_t;



//////////////////////////////////////////////////////////////////////////////
//	VARIABLE DEFINITIONS
//////////////////////////////////////////////////////////////////////////////

static LED_STATUS		leddrv_LEDStatus[LED_TYPE_COUNT];
static LED_BLINK_ALARM	leddrv_LEDBlinkAlarmTable[LED_TYPE_COUNT] =
{
	{	ledtype_StatusLED_Green,	"LED_Green"	},
};

static PLEDHANDLER		leddrv_BlinkCBR[LED_TYPE_COUNT];
static void				*leddrv_BlinkCBRUserInstance[LED_TYPE_COUNT];
static LedBlinkNotify_t s_LEDBlinkNotify[LED_TYPE_COUNT];

static LED_INFO s_LedInfo[LED_TYPE_COUNT] = {
    {
    	.GpioGroup  = GPIO_LED_STATUS_GREEN_GROUP,
    	.GpioBit    = GPIO_LED_STATUS_GREEN_BIT,
    	.Logic      = GPIO_LED_STATUS_GREEN_LOGIC,
    	.Enable     = TRUE,
    	.EnableBeep = FALSE,
    	.LedType    = ledtype_StatusLED_Green,
    	.BlinkAlarm = OS_NULL_INT,
    },

};

static uint32_t s_PwmDuty = 50;

//////////////////////////////////////////////////////////////////////////////
//	FUNCTION DECLARATIONS
//////////////////////////////////////////////////////////////////////////////

static void LEDBlinkTimer(UINT32 pUserInstance);
static void LEDOnOff(LED_TYPE Type, bool IsOn);
static void SetPwmDuty(LED_TYPE LedType, uint32_t Duty);


//////////////////////////////////////////////////////////////////////////////
//	FUNCTION DEFINITIONS
//////////////////////////////////////////////////////////////////////////////


void InitializeLEDDriver(void)
{
	uint32_t i = 0;
    uint16_t id = 0;
    uint16_t uwRet;
    
	for (i = 0; i < LED_TYPE_COUNT; i++)
	{
        if ( OS_NULL_INT == s_LedInfo[i].BlinkAlarm )
        {
            //I43_SelectGPIOEx( s_LedInfo[i].GpioGroup, s_LedInfo[i].GpioBit, I43_GPIO_GPIO       );
            //I43_ConfigGPIOEx( s_LedInfo[i].GpioGroup, s_LedInfo[i].GpioBit, I43_GPIO_CONFIG_OUT );
            //I43_SetGPIOEx   ( s_LedInfo[i].GpioGroup, s_LedInfo[i].GpioBit, !s_LedInfo[i].Logic );

            uwRet = LOS_SwtmrCreate( LED_BLINK_TIME, 
                                     LOS_SWTMR_MODE_PERIOD, 
                                     LEDBlinkTimer, 
                                     &id, 
                                     leddrv_LEDBlinkAlarmTable[i].LedType );
            if( uwRet != LOS_OK )
            {
                LOG("LOS_SwtmrCreate fail 0x%x\n", uwRet);
                return ;
            }
            
            s_LedInfo[i].BlinkAlarm = id;
            LOG("LOS_SwtmrCreate id = %d\n", s_LedInfo[i].BlinkAlarm );
        }
	}

	for (i = 0; i < LED_TYPE_COUNT; i++)
	{
		leddrv_LEDStatus[i].IsOn			= FALSE;
		leddrv_LEDStatus[i].IsLocked		= FALSE;
		leddrv_LEDStatus[i].Millisec		= 0;
		leddrv_LEDStatus[i].CurrentTime		= 0;
		leddrv_LEDStatus[i].OnOffCount		= 0;
		leddrv_LEDStatus[i].OnOffCountMax	= 0;
		leddrv_BlinkCBR[i]					= NULL;

		s_LEDBlinkNotify[i].p_cbr = NULL;
		s_LEDBlinkNotify[i].p_user_instance = NULL;

        // TODO:
		if(s_LedInfo[i].GpioGroup == GPIOD)
        {
			SetPwmDuty((LED_TYPE)i, s_PwmDuty);
		}
        else
        {
            LOG("s_LedInfo[i].GpioGroup 0x%x %d\n", s_LedInfo[i].GpioGroup , s_LedInfo[i].GpioBit);
            LOG("GPIOB 0x%x \n", GPIOB, s_LedInfo[i].GpioBit);
			//I43_SelectGPIOEx( s_LedInfo[i].GpioGroup, s_LedInfo[i].GpioBit, I43_GPIO_GPIO );
			//I43_ConfigGPIOEx( s_LedInfo[i].GpioGroup, s_LedInfo[i].GpioBit, I43_GPIO_CONFIG_OUT );
		}
	}

#if DEBUG_ENABLE
	RegisterLEDDriverCommand();
#endif

    //LED1_ON;
    LOG("InitializeLEDDriver\n");
    LEDDrv_BlinkDuty(ledtype_StatusLED_Green,ledblfreq_1_2, ledblduty_20);
}

//////////////////////////////////////////////////////////////////////////////

void LEDDrv_On(LED_TYPE Type)
{
	if (leddrv_LEDStatus[Type].IsLocked)
	{
		return;
	}

	leddrv_LEDStatus[Type].IsOn			= TRUE;
	leddrv_LEDStatus[Type].Millisec		= 0;
	leddrv_LEDStatus[Type].CurrentTime	= 0;

	LEDOnOff(Type, TRUE);
}

//////////////////////////////////////////////////////////////////////////////

void LEDDrv_Off(LED_TYPE Type)
{
	if (leddrv_LEDStatus[Type].IsLocked)
	{
		return;
	}

	leddrv_LEDStatus[Type].IsOn			= FALSE;
	leddrv_LEDStatus[Type].Millisec		= 0;
	leddrv_LEDStatus[Type].CurrentTime	= 0;

	LEDOnOff(Type, FALSE);

	if(s_LedInfo[Type].BlinkAlarm != OS_NULL_INT)
	{	// Stop blink alarm
		//StopAlarm(s_LedInfo[Type].BlinkAlarm);
		LOS_SwtmrStop(s_LedInfo[Type].BlinkAlarm);
	}
}

//////////////////////////////////////////////////////////////////////////////

void LEDDrv_Blink(LED_TYPE Type, uint16_t Freq)
{
    uint16_t uwRet = 0;
	bool is_on = !(leddrv_LEDStatus[Type].IsOn);

	//assert(Freq != 0);

	if (leddrv_LEDStatus[Type].IsLocked)
	{
		return;
	}

	leddrv_LEDStatus[Type].IsOn			= is_on;
	leddrv_LEDStatus[Type].CurrentTime	= 0;
	leddrv_LEDStatus[Type].Millisec		= 50 / Freq;	// Millisec = (1000 / LED_BLINK_TIME / 2) / Frequency
	leddrv_LEDStatus[Type].MillisecOn	= leddrv_LEDStatus[Type].Millisec;
	leddrv_LEDStatus[Type].MillisecOff	= leddrv_LEDStatus[Type].Millisec;
	leddrv_LEDStatus[Type].OnOffCountMax= 0;

	LEDOnOff(Type, is_on);
	if(s_LEDBlinkNotify[Type].p_cbr != NULL)
	{
		(*s_LEDBlinkNotify[Type].p_cbr)(s_LEDBlinkNotify[Type].p_user_instance);
	}

	if(s_LedInfo[Type].BlinkAlarm != OS_NULL_INT)
	{	// Start blink alarm
		//StartAlarm(s_LedInfo[Type].BlinkAlarm);
		uwRet = LOS_SwtmrStart(s_LedInfo[Type].BlinkAlarm);
		if ( uwRet != LOS_OK )
		{
		    LOG("LOS_SwtmrStart fail 0x%x\n", uwRet);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////

void LEDDrv_BlinkDuty(LED_TYPE Type, LEDBL_FREQ Freq, LEDBL_DUTY Duty)
{
	uint16_t frequency;

	switch (Freq)
	{
		case ledblfreq_1_2 :
			frequency = 500;
			break;
		case ledblfreq_1 :
			frequency = 1000;
			break;
		case ledblfreq_2 :
			frequency = 2000;
			break;
		case ledblfreq_4 :
			frequency = 4000;
			break;
		case ledblfreq_8 :
			frequency = 8000;
			break;
		default :
			//assert(FALSE);
			break;
	}
	LEDDrv_BlinkDutyWithCount(Type, frequency, Duty, -1);
}

//////////////////////////////////////////////////////////////////////////////

void LEDDrv_BlinkDutyWithCount(LED_TYPE Type, uint16_t Freq, LEDBL_DUTY Duty, short OnOffCntMax)
{
	uint16_t millisec_per_cycle[LED_TYPE_COUNT];
	uint16_t duty_local[LED_TYPE_COUNT];
    uint16_t uwRet;
	if (leddrv_LEDStatus[Type].IsLocked)
	{
		return;
	}

	if ((Freq == 0) || (Freq > 10000))
	{
		//assert(0);
	}
	else
	{
		millisec_per_cycle[Type] = 1000 * 1000 / Freq;	//
	}

	switch (Duty)
	{
		case ledblduty_0 :
			duty_local[Type] = 0;
			break;

		case ledblduty_20 :
			duty_local[Type] = 20;
			break;

		case ledblduty_25 :
			duty_local[Type] = 25;
			break;

		case ledblduty_50 :
			duty_local[Type] = 50;
			break;

		case ledblduty_75 :
			duty_local[Type] = 75;
			break;

		case ledblduty_100 :
			duty_local[Type] = 100;
			break;

		default :
			//assert(0);
			break;
	}

	leddrv_LEDStatus[Type].OnOffCountMax = OnOffCntMax;
	leddrv_LEDStatus[Type].OnOffCount	 = 0;

	leddrv_LEDStatus[Type].CurrentTime	= 0;
	if (Duty == ledblduty_100)
	{
		leddrv_LEDStatus[Type].IsOn		= TRUE;
		leddrv_LEDStatus[Type].Millisec	= 0;

	}
	else if (Duty == ledblduty_0)
	{
		leddrv_LEDStatus[Type].IsOn		= FALSE;
		leddrv_LEDStatus[Type].Millisec	= 0;
	}
	else
	{
		leddrv_LEDStatus[Type].IsOn			= TRUE;
		leddrv_LEDStatus[Type].Millisec		= (millisec_per_cycle[Type] * duty_local[Type]) / 100 / LED_BLINK_TIME;
		leddrv_LEDStatus[Type].MillisecOn	= leddrv_LEDStatus[Type].Millisec;
		leddrv_LEDStatus[Type].MillisecOff	= ( millisec_per_cycle[Type] * (100 - duty_local[Type]) ) / 100 / LED_BLINK_TIME;
	}

	LEDOnOff(Type, leddrv_LEDStatus[Type].IsOn);
	if(s_LEDBlinkNotify[Type].p_cbr != NULL)
	{
		(*s_LEDBlinkNotify[Type].p_cbr)(s_LEDBlinkNotify[Type].p_user_instance);
	}

    //LOG("LOS_SwtmrStart %d\n", s_LedInfo[Type].BlinkAlarm );

	if(s_LedInfo[Type].BlinkAlarm != OS_NULL_INT )
	{	// Start blink alarm
		//StartAlarm(s_LedInfo[Type].BlinkAlarm);
		uwRet = LOS_SwtmrStart(s_LedInfo[Type].BlinkAlarm);
		if ( uwRet != LOS_OK )
		{
		    LOG("LOS_SwtmrStart fail 0x%x\n", uwRet);
		}
	}

}

//////////////////////////////////////////////////////////////////////////////

void LEDDrv_Lock(LED_TYPE Type)
{
	leddrv_LEDStatus[Type].IsLocked = TRUE;
}

//////////////////////////////////////////////////////////////////////////////

void LEDDrv_Unlock(LED_TYPE Type)
{
	leddrv_LEDStatus[Type].IsLocked = FALSE;
}

void LEDDrv_Enable(LED_TYPE Type)
{
	s_LedInfo[Type].Enable = TRUE;
}

void LEDDrv_Disable(LED_TYPE Type)
{
	s_LedInfo[Type].Enable = FALSE;
}

void LEDDrv_EnableBeep(LED_TYPE Type)
{
	s_LedInfo[Type].EnableBeep = TRUE;
}

void LEDDrv_DisableBeep(LED_TYPE Type)
{
	s_LedInfo[Type].EnableBeep = FALSE;
}

//////////////////////////////////////////////////////////////////////////////

static void LEDBlinkTimer(UINT32 pUserInstance)
{
	LED_TYPE led_type = (LED_TYPE )pUserInstance;

	if (leddrv_LEDStatus[led_type].Millisec > 0)
	{
		leddrv_LEDStatus[led_type].CurrentTime++;

		if (leddrv_LEDStatus[led_type].CurrentTime >= leddrv_LEDStatus[led_type].Millisec)
		{
			if (leddrv_LEDStatus[led_type].IsOn == FALSE)
			{
				if (leddrv_LEDStatus[led_type].OnOffCountMax > 0)
				{
					leddrv_LEDStatus[led_type].OnOffCount++;		//
					if (leddrv_LEDStatus[led_type].OnOffCount >= leddrv_LEDStatus[led_type].OnOffCountMax)
					{
						leddrv_LEDStatus[led_type].OnOffCount = 0;
						if (leddrv_BlinkCBR[led_type] != NULL)
						{
							LEDDrv_Off(led_type);
							(*leddrv_BlinkCBR[led_type])(leddrv_BlinkCBRUserInstance[led_type]);
							return;
						}
					}
				}
				if(s_LEDBlinkNotify[led_type].p_cbr != NULL)
				{
					(*s_LEDBlinkNotify[led_type].p_cbr)(s_LEDBlinkNotify[led_type].p_user_instance);
				}
			}

			leddrv_LEDStatus[led_type].IsOn = !(leddrv_LEDStatus[led_type].IsOn);
			LEDOnOff(led_type, leddrv_LEDStatus[led_type].IsOn);
			leddrv_LEDStatus[led_type].CurrentTime	= 0;
			if (leddrv_LEDStatus[led_type].IsOn)
			{
				leddrv_LEDStatus[led_type].Millisec = leddrv_LEDStatus[led_type].MillisecOn;
			}
			else
			{
				leddrv_LEDStatus[led_type].Millisec = leddrv_LEDStatus[led_type].MillisecOff;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////

static void LEDOnOff(LED_TYPE Type, bool IsOn)
{
    //LOG("LEDOnOff type %d - %d\n", Type, IsOn);
	if( s_LedInfo[Type].Enable)
	{
		switch (Type)
		{
			case ledtype_StatusLED_Green :
				if(s_LedInfo[Type].Logic == TRUE)
				{
				    if ( IsOn )
				    {
				        //LOG("LEDOnOff type %d - %d\n", Type, IsOn);
				        //LED1_ON;
				        digitalLo(s_LedInfo[ledtype_StatusLED_Green].GpioGroup, s_LedInfo[ledtype_StatusLED_Green].GpioBit);
				    }
				    else
				    {
				        //LED1_OFF;
				        digitalHi(s_LedInfo[ledtype_StatusLED_Green].GpioGroup, s_LedInfo[ledtype_StatusLED_Green].GpioBit);
				    }
					//I43_SetGPIOEx(s_LedInfo[ledtype_StatusLED_Green].GpioGroup, s_LedInfo[ledtype_StatusLED_Green].GpioBit, IsOn ? I43_TRUE : I43_FALSE);
					//I43_SetGPIOEx(s_LedInfo[ledtype_StatusLED_Red].GpioGroup, s_LedInfo[ledtype_StatusLED_Red].GpioBit, I43_FALSE);
				}
				else
				{
					//I43_SetGPIOEx(s_LedInfo[ledtype_StatusLED_Green].GpioGroup, s_LedInfo[ledtype_StatusLED_Green].GpioBit, IsOn ? I43_FALSE : I43_TRUE);
					//I43_SetGPIOEx(s_LedInfo[ledtype_StatusLED_Red].GpioGroup, s_LedInfo[ledtype_StatusLED_Red].GpioBit, I43_TRUE);
				}
				break;

			case ledtype_StatusLED_Red :
				if(s_LedInfo[Type].Logic == TRUE)
				{
					//I43_SetGPIOEx(s_LedInfo[ledtype_StatusLED_Red].GpioGroup, s_LedInfo[ledtype_StatusLED_Red].GpioBit, IsOn ? I43_TRUE : I43_FALSE);
					//I43_SetGPIOEx(s_LedInfo[ledtype_StatusLED_Green].GpioGroup, s_LedInfo[ledtype_StatusLED_Green].GpioBit, I43_FALSE);
				}
				else
				{
					//I43_SetGPIOEx(s_LedInfo[ledtype_StatusLED_Red].GpioGroup, s_LedInfo[ledtype_StatusLED_Red].GpioBit, IsOn ? I43_FALSE : I43_TRUE);
					//I43_SetGPIOEx(s_LedInfo[ledtype_StatusLED_Green].GpioGroup, s_LedInfo[ledtype_StatusLED_Green].GpioBit, I43_TRUE);
				}

				break;

			default :
				//assert(FALSE);
                break;
		}
	}

	if ( s_LedInfo[Type].EnableBeep )
	{
		if ( IsOn )
		{
			//SoundDrv_BeepOn(BEEP_TYPE_normal);
		}
		else
		{
			//SoundDrv_BeepOff(BEEP_TYPE_normal);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////

void LEDDrv_RegisterBlinkWithCountCallBack(LED_TYPE LedType, PLEDHANDLER pHandler, void *pUserInstance)
{
	leddrv_BlinkCBR[LedType]				= pHandler;
	leddrv_BlinkCBRUserInstance[LedType]	= pUserInstance;
}


void LEDDrv_UnregisterBlinkWithCountCallBack(LED_TYPE LedType)
{
	leddrv_BlinkCBR[LedType] = NULL;
}

void LEDDrv_RegisterNormalBlinkCallBack(LED_TYPE LedType, PLEDHANDLER pHandler, void *pUserInstance)
{
	s_LEDBlinkNotify[LedType].p_cbr = pHandler;
	s_LEDBlinkNotify[LedType].p_user_instance = pUserInstance;
}

void LEDDrv_UnregisterNormalBlinkCallBack(LED_TYPE LedType)
{
	s_LEDBlinkNotify[LedType].p_cbr = NULL;
	s_LEDBlinkNotify[LedType].p_user_instance = NULL;
}

static void SetPwmDuty(LED_TYPE LedType, uint32_t Duty)
{
	//I43_SetCounter(s_LedInfo[LedType].GpioBit, I43_COUNTER_DISABLE);
	//I43_ConfigCounter(s_LedInfo[LedType].GpioBit, (I43_MODE_DUTY_CYLE | I43_MODE_CONTINUOUS), LED_PWM_FREQ, Duty);
	//I43_SetCounter(s_LedInfo[LedType].GpioBit, I43_COUNTER_ENABLE);

}

void LEDDrv_ChangePwmDuty(uint32_t Duty)
{
	s_PwmDuty = Duty;
	SetPwmDuty(ledtype_StatusLED_Red, s_PwmDuty);
}



