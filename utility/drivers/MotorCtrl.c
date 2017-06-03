#include <stm32f10x.h>
#include <cmsis_os.h>
#include <los_typedef.h>
#include <l293d.h>
#include <motorctrl.h>
#include <motorctrl_cmd.h>
#include <debug.h>

void MOTORCtrl_Init(void)
{
    l293dInit();
    RegisterMotorDrvCommand();

}

void MOTORCtrl_Rotate(MOTOR_TYPE motor, ROTATE_TYPE rotate )
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

void MOTORCtrl_StopAll(void )
{
    L293D_INx_DISABLE_ALL;
}

void MOTORCtrl_CW(MOTOR_TYPE motor)
{
}

void MOTORCtrl_CCW(MOTOR_TYPE motor)
{

}

void MOTORCtrl_DeInit(void)
{
    
}

void ygGoForward(void)
{
    TRACE("ygGoForward\n");
    MOTORCtrl_Rotate(  motortype_Front_Left , rotateType_CW);
    MOTORCtrl_Rotate(  motortype_Front_Right, rotateType_CW);

    MOTORCtrl_Rotate(  motortype_Rear_Left  , rotateType_CW);
    MOTORCtrl_Rotate(  motortype_Rear_Right , rotateType_CW);
}

void ygGoBackward(void)
{
    MOTORCtrl_Rotate(motortype_Front_Left  ,  rotateType_CCW );
    MOTORCtrl_Rotate(motortype_Front_Right ,  rotateType_CCW );

    MOTORCtrl_Rotate(motortype_Rear_Left   ,  rotateType_CCW );
    MOTORCtrl_Rotate(motortype_Rear_Right  ,  rotateType_CCW );
}

void ygTurnLeft(void)
{
    MOTORCtrl_Rotate( motortype_Front_Left,  rotateType_CCW  );
    MOTORCtrl_Rotate( motortype_Rear_Left ,  rotateType_CCW  );

    MOTORCtrl_Rotate(motortype_Front_Right,  rotateType_CW );
    MOTORCtrl_Rotate(motortype_Rear_Right ,  rotateType_CW );
}

void ygTurnRight(void)
{
    MOTORCtrl_Rotate(motortype_Front_Left  ,  rotateType_CW );
    MOTORCtrl_Rotate(motortype_Rear_Left   ,  rotateType_CW );

    MOTORCtrl_Rotate( motortype_Front_Right,  rotateType_CCW );
    MOTORCtrl_Rotate( motortype_Rear_Right ,  rotateType_CCW );
}

void ygStop(void)
{
    
}

