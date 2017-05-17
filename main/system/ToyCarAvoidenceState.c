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
static void OnIdleStatus( void );
static void OnComplete( void );

// VARIABLE DEFINITIONS


// FUNCTION DEFINITIONS

void AvoidenceState_Init( void )
{
    ToyCarManager_RegisterState( toyCarState_Avoidence, "Avoidence", OnStart, OnStop, OnEventHandler );
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

