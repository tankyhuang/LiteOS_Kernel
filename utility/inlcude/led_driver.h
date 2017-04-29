#ifndef _LED_DRIVER_H_
#define _LED_DRIVER_H_

#include "cmsis_os.h"
#include "bsp_led.h"

#ifdef __cplusplus
extern "C"
{
#endif

    #

//////////////////////////////////////////////////////////////////////////////
//	CONSTANT DEFINITIONS
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
//	TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////////

typedef void (*PLEDHANDLER)(void *pUserInstance);
typedef void	*HALARM;

typedef enum LED_TYPE
{
	ledtype_StatusLED_Unknown = -1,
	ledtype_StatusLED_Green,
	ledtype_StatusLED_Red,
//	ledtype_PowerLED,

	LED_TYPE_COUNT
} LED_TYPE;

typedef enum LEDBL_FREQ
{
	ledblfreq_1_2,		// 0.5Hz
	ledblfreq_1,		// 1Hz
	ledblfreq_2,		// 2Hz
	ledblfreq_4,		// 4Hz
	ledblfreq_8,		// 8Hz

	LEDBL_FREQ_COUNT
} LEDBL_FREQ;

typedef enum LEDBL_DUTY
{
	ledblduty_0,		// 0%
	ledblduty_20,		// 20%
	ledblduty_25,		// 25%
	ledblduty_50,		// 50%
	ledblduty_75,		// 75%
	ledblduty_100,		// 100%

	LEDBL_DUTY_COUNT
} LEDBL_DUTY;

typedef struct {
	GPIO_TypeDef        *GpioGroup;
	uint16_t 			GpioBit;
	BOOL				Logic;
	BOOL 				Enable;
	BOOL 				EnableBeep;	// Blink with beep on
	LED_TYPE 			LedType;
	UINT32 				BlinkAlarm;
} LED_INFO;


//////////////////////////////////////////////////////////////////////////////
//	FUNCTION DECLARATIONS
//////////////////////////////////////////////////////////////////////////////

void InitializeLEDDriver(void);
void LEDDrv_On(LED_TYPE Type);
void LEDDrv_Off(LED_TYPE Type);
void LEDDrv_Blink(LED_TYPE Type, uint16_t Freq);
void LEDDrv_BlinkDuty(LED_TYPE Type, LEDBL_FREQ Freq, LEDBL_DUTY  Duty);
void LEDDrv_BlinkDutyWithCount(LED_TYPE Type, uint16_t Freq, LEDBL_DUTY	Duty, short OnOffCntMax);
void LEDDrv_Lock(LED_TYPE Type);
void LEDDrv_Unlock(LED_TYPE Type);
void LEDDrv_RegisterBlinkWithCountCallBack(LED_TYPE LedType, PLEDHANDLER pHandler, void *pUserInstance);
void LEDDrv_UnregisterBlinkWithCountCallBack(LED_TYPE LedType);
void LEDDrv_RegisterNormalBlinkCallBack(LED_TYPE LedType, PLEDHANDLER pHandler, void *pUserInstance);
void LEDDrv_UnregisterNormalBlinkCallBack(LED_TYPE LedType);
void LEDDrv_ChangePwmDuty(uint32_t Duty);

void LEDDrv_Enable(LED_TYPE Type);
void LEDDrv_Disable(LED_TYPE Type);

void LEDDrv_EnableBeep(LED_TYPE Type);
void LEDDrv_DisableBeep(LED_TYPE Type);

#ifdef __cplusplus
}
#endif

#endif
