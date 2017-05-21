#include <stm32f10x.h>
#include <cmsis_os.h>
#include <los_typedef.h>
#include <l293d.h>
#include <motorctrl.h>


void motorCtrlInit(void)
{

}

void motorCtrl_Rotate(MOTOR_TYPE motor, ROTATE_TYPE rotate )
{
    
    switch ( motor )
    {
        case motortype_Front_Left:
        case motortype_Rear_Left:
            (rotate == rotateType_CW)? L293D_IN1_ENABLE : L293D_IN2_ENABLE;
            break;

        case motortype_Front_Right:
        case motortype_Rear_Right:
            (rotate == rotateType_CW)? L293D_IN3_ENABLE : L293D_IN4_ENABLE;
            break;

        default:
            break;
    }
}

void motorCtrl_StopAll(void )
{
    L293D_INx_DISABLE_ALL;
}

void motorCtrl_CW(MOTOR_TYPE motor)
{
}

void motorCtrl_CCW(MOTOR_TYPE motor)
{

}

void motorCtrlDeInit(void)
{
    
}

void ygGoForward(void)
{
    motorCtrl_Rotate(  motortype_Front_Left , rotateType_CW);
    motorCtrl_Rotate(  motortype_Front_Right, rotateType_CW);

    motorCtrl_Rotate(  motortype_Rear_Left  , rotateType_CW);
    motorCtrl_Rotate(  motortype_Rear_Right , rotateType_CW);
}

void ygGoBackward(void)
{
    motorCtrl_Rotate(motortype_Front_Left  ,  rotateType_CCW );
    motorCtrl_Rotate(motortype_Front_Right ,  rotateType_CCW );

    motorCtrl_Rotate(motortype_Rear_Left   ,  rotateType_CCW );
    motorCtrl_Rotate(motortype_Rear_Right  ,  rotateType_CCW );
}

void ygTurnLeft(void)
{
    motorCtrl_Rotate( motortype_Front_Left,  rotateType_CCW  );
    motorCtrl_Rotate( motortype_Rear_Left ,  rotateType_CCW  );

    motorCtrl_Rotate(motortype_Front_Right,  rotateType_CW );
    motorCtrl_Rotate(motortype_Rear_Right ,  rotateType_CW );
}

void ygTurnRight(void)
{
    motorCtrl_Rotate(motortype_Front_Left  ,  rotateType_CW );
    motorCtrl_Rotate(motortype_Rear_Left   ,  rotateType_CW );

    motorCtrl_Rotate( motortype_Front_Right,  rotateType_CCW );
    motorCtrl_Rotate( motortype_Rear_Right ,  rotateType_CCW );
}

void ygStop(void)
{
    
}

