#ifndef __MOTOR_CTRL_H_
#define __MOTOR_CTRL_H_

#include "cmsis_os.h"
#include "gpio_conf.h"


#ifdef __cplusplus
extern "C"
{
#endif

typedef enum MOTOR_TYPE
{
	motortype_Unknown = -1,
	motortype_Front_Left,
    motortype_Front_Right,
    motortype_Rear_Left,
    motortype_Rear_Right,

	MOTOR_TYPE_COUNT
} MOTOR_TYPE;

typedef enum ROTATE_TYPE
{
	rotateType_Unknown = -1,
	// clockwise
	rotateType_CW,

    // counter-clockwise
    rotateType_CCW,

	ROTATE_TYPE_COUNT
} ROTATE_TYPE;

//////////////////////////////////////////////////////////////////////////////
//	CONSTANT DEFINITIONS
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
//	TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////
//	FUNCTION DECLARATIONS
//////////////////////////////////////////////////////////////////////////////


void MOTORCtrl_Init(void);
void MOTORCtrl_UnInit(void);



void MOTORCtrl_Rotate(MOTOR_TYPE motor, ROTATE_TYPE rotate);

void MOTORCtrl_StopAll(void );



void ygGoForward(void);

void ygGoBackward(void);

void ygTurnLeft(void);

void ygTurnRight(void);

void ygStop(void);

#ifdef __cplusplus
}
#endif

#endif  // __MOTOR_CTRL_H_
