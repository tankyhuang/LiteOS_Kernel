#include "stm32f10x.h"
#include "types.h"
#include "ToyCarManager.h"
#include "debug.h"

// TYPE DEFINITIONS

// FUNCTION DECLARATIONS

static void OnStart( void );
static void OnStop( void );
static ERROR_T OnEventHandler( TOYCAR_EVENT_ID ID, void *pParam, P_TOYCAR_COMPLETE_CBR pCompleteCBR, void *pUserInstance );
static void OnOff( P_TOYCAR_COMPLETE_CBR pCompleteCBR, void *pUserInstance );
static void OnS0( P_TOYCAR_COMPLETE_CBR pCompleteCBR, void *pUserInstance );
static void OnIdleStatus( void );
static void OnComplete( void );
static void OnParameterSetting( void );


static void OnDisplayDebugInfo( void );

// VARIABLE DEFINITIONS

static bool s_IsIdleCompleted = false;
static bool s_IsS0Started = false;

// FUNCTION DEFINITIONS

/////////////////////////////////////////////////////////////////////////////////////////



void OffState_Init( void )
{
    ToyCarManager_RegisterState( toyCarState_Off, "Off", OnStart, OnStop, OnEventHandler );
}

static void OnStart( void )
{

}

static void OnStop( void )
{

}

static void OnOff( P_TOYCAR_COMPLETE_CBR pCompleteCBR, void *pUserInstance )
{
    ERROR_T result = ERROR;


    TRACE( "[Capture] : %s\r\n", ( ( result != OK ) ? "OnOff --- Can not accept" : "Idle to Off" ) );
    //CaptureDataStorage_NotifyCaptureCompleteCBR( result );
}

static void OnS0( P_TOYCAR_COMPLETE_CBR pCompleteCBR, void *pUserInstance )
{

}

static void OnIdleStatus( void )
{

}




static void OnParameterSetting( void )
{
 
}


static void OnComplete( void )
{
    if( !s_IsIdleCompleted )
    {
        return;
    }

    //CaptureDataStorage_NotifyCaptureCompleteCBR( OK );
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
            OnS0( pCompleteCBR, pUserInstance );
            break;

        default:
            result = ERROR;
            break;
    }
    return result;
}


static void OnDisplayDebugInfo( void )
{
    //DEBUG_INFO_STRING( "IsIdle", ( ( CaptureDataStorage_IsIdle() ) ? "YES" : "NO" ) );
    //DEBUG_INFO_STRING( "IsIdleCompleted", ( ( s_IsIdleCompleted ) ? "YES" : "NO" ) );
    //DEBUG_INFO_STRING( "IsS0Started", ( ( s_IsS0Started ) ? "YES" : "NO" ) );
}

