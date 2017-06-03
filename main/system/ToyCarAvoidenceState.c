#include "stm32f10x.h"
#include "types.h"
#include "los_swtmr.h"
#include "SystemMonitor.h"
#include "ToyCarManager.h"
#include "MotorCtrl.h"
#include "debug.h"


// TYPE DEFINITIONS

// FUNCTION DECLARATIONS

static void OnStart( void );
static void OnStop( void );
static ERROR_T OnEventHandler( TOYCAR_EVENT_ID ID, void *pParam, P_TOYCAR_COMPLETE_CBR pCompleteCBR, void *pUserInstance );
static void OnOff( P_TOYCAR_COMPLETE_CBR pCompleteCBR, void *pUserInstance );
static void OnIdleStatus( void );
static void OnComplete( void );
static void _TimerHandler(UINT32 pUserInstance);

// VARIABLE DEFINITIONS
static SYS_RED_RAY_STATUS s_RedRayStatus = sysRedRayStatus_None;
static uint16_t           s_TimerId = 0;
static uint16_t           s_TimerMaxCount = 0;
static uint16_t           s_TimerCount    = 0;
static uint16_t           s_CarTask       = 0;

// FUNCTION DEFINITIONS

void AvoidenceState_Init( void )
{
    ToyCarManager_RegisterState( toyCarState_Avoidence, "Avoidence", OnStart, OnStop, OnEventHandler );
}


static void OnStart( void )
{
    uint32_t uwRet;
    
    s_TimerMaxCount=8;
    uwRet = LOS_SwtmrCreate( 100, 
                             LOS_SWTMR_MODE_PERIOD, 
                             _TimerHandler, 
                             &s_TimerId, 
                             0 );
    ASSERT( LOS_OK == uwRet );                             
}

static void OnStop( void )
{
}

static void _TimerHandler(UINT32 pUserInstance)
{
    if ( s_TimerCount++ >= s_TimerMaxCount )
    {
        s_TimerCount = 0;
        if ( s_CarTask == 1 )
        {
            MOTORCtrl_StopAll();
            ygTurnRight();
            s_CarTask = 0;
        }
        else if (s_CarTask == 2)
        {
            s_TimerCount = 0;
            s_CarTask = 0;
            MOTORCtrl_StopAll();
            ygTurnLeft();
        }
        else if (s_CarTask == 3)
        {
            MOTORCtrl_StopAll();
            ygTurnLeft();
            s_CarTask = 2;
        }
        else
        {
            MOTORCtrl_StopAll();
            ygGoForward();
            s_CarTask = 0;
            LOS_SwtmrStop(s_TimerId);
        }
    }
}

static void OnOff( P_TOYCAR_COMPLETE_CBR pCompleteCBR, void *pUserInstance )
{
    ERROR_T result = ERROR;


    TRACE( "[Capture] : %s\r\n", ( ( result != OK ) ? "OnOff --- Can not accept" : "Idle to Off" ) );
    //CaptureDataStorage_NotifyCaptureCompleteCBR( result );
}

static void OnRedRay( SYS_RED_RAY_STATUS status )
{
    TRACE("OnRedRay %d\n", status);   

    ASSERT( status < SYS_RED_RAY_STATUS_COUNT );
    
    s_RedRayStatus = status;

    //ygStop();
    MOTORCtrl_StopAll();
    
    switch( s_RedRayStatus )
    {
        TRACE("s_RedRayStatus %d\n", s_RedRayStatus);   
        
        case sysRedRayStatus_None:
            ygGoForward();
            break;
        case sysRedRayStatus_L_True:
            ygGoBackward();
            //LOS_TaskDelay(500);
            s_CarTask = 1;
            LOS_SwtmrStart(s_TimerId);
            //ygTurnRight();
            //LOS_TaskDelay(500);
            break;
        case sysRedRayStatus_R_True:
            ygGoBackward();
            s_CarTask = 2;
            //LOS_TaskDelay(500);
            //ygTurnLeft();
            //LOS_TaskDelay(500);
            
            LOS_SwtmrStart(s_TimerId);
            break;
        case sysRedRayStatus_L_R_True:
            s_CarTask = 3;
            ygGoBackward();            
            LOS_SwtmrStart(s_TimerId);
            break;
        default:
            ASSERT(FALSE);
            break;
    }
}

static ERROR_T OnEventHandler( TOYCAR_EVENT_ID ID, void *pParam, P_TOYCAR_COMPLETE_CBR pCompleteCBR, void *pUserInstance )
{
    ERROR_T result = OK;

    switch( ID )
    {
        case toyCarEvent_Off:
            OnOff( pCompleteCBR, pUserInstance );
            break;
        case toyCarEvent_S0:
           
            break;

        case toyCarEvent_RedRay:
            TRACE("toyCarEvent_RedRay \n");
            OnRedRay( (SYS_RED_RAY_STATUS)pParam );
            break;

        case toyCarEvent_OnCreate:
            TRACE("toyCarEvent_OnCreate %d\n", (uint32_t)pParam);
            ygGoForward();
            //ygGoBackward();
            //ygTurnLeft();
            //ygTurnRight();
            
            break;
        default:
            result = ERROR;
            break;
    }
    return result;
}


static void OnComplete( void )
{
    //CaptureDataStorage_NotifyCaptureCompleteCBR( OK );
}


static void OnDisplayDebugInfo( void )
{
    //DEBUG_INFO_STRING( "IsLiveViewStarted", ( ( s_IsLiveViewStarted ) ? "YES" : "NO" ) );
    //DEBUG_INFO_STRING( "IsOperationBusy", ( ( s_IsOperationBusy ) ? "YES" : "NO" ) );
}

