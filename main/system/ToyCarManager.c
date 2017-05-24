#include "stm32f10x.h"
#include "types.h"
#include "ToyCarManager.h"
#include "ToyCarState.h"
#include "debug.h"


typedef struct
{
    P_TOYCAR_COMPLETE_CBR pCompleteCBR;
    void *pUserInstance;
} OPERATION_PARAM;

// FUNCTION DEFINITIONS
static void OnEventHandler( TOYCAR_EVENT_ID EventID, void *pParam, P_TOYCAR_COMPLETE_CBR pCompleteCBR, void *pUserInstance );
static void OnOff( void *pParameter );
static void OnIdle( void *pParameter );
static void OnS0( void *pParameter );
static void OnS1( void *pParameter );


// VARIABLE DEFINITIONS

static STOYCAR_STATE s_tToyCarStateTable[toyCarState_StateNumber];
static TOYCAR_STATE_ID s_CurrentToyCarState = toyCarState_Invalid;


void ToyCarManager_Init( void )
{
    int i = 0;

    for( i=0; i<toyCarState_StateNumber; i++ )
    {
        s_tToyCarStateTable[i].StateID    = toyCarState_Invalid;
        s_tToyCarStateTable[i].pStateName = "";
        s_tToyCarStateTable[i].pOnStart   = NULL;
        s_tToyCarStateTable[i].pOnStop    = NULL;
        s_tToyCarStateTable[i].pOnEventHandler = NULL;
    }

    s_CurrentToyCarState = toyCarState_Off;
}

void ToyCarManager_OnEventHandler( TOYCAR_EVENT_ID EventID, void *pParam, P_TOYCAR_COMPLETE_CBR pCompleteCBR, void *pUserInstance )
{
    OnEventHandler( EventID, pParam, pCompleteCBR, pUserInstance );
}

static void OnEventHandler( TOYCAR_EVENT_ID EventID, void *pParam, P_TOYCAR_COMPLETE_CBR pCompleteCBR, void *pUserInstance )
{
    ERROR_T result;

    assert_param( s_tToyCarStateTable[s_CurrentToyCarState].StateID!= toyCarState_Invalid ); // Capture is not ready.

    result = (*s_tToyCarStateTable[s_CurrentToyCarState].pOnEventHandler)( EventID, pParam, pCompleteCBR, pUserInstance );

    if( result != OK )
    {
        ToyCarState_OnEventHandler( EventID, pParam, pCompleteCBR, pUserInstance );
    }
}

void CaptureManager_OnRegisterStatusNotifyCBR( P_TOYCAR_STATUS_NOTIFY_CBR pStatusNotifyCBR, void *pUserInstance )
{
}

void CaptureManager_OnUnregisterStatusNotifyCBR( void )
{
}

void CaptureManager_OnRegisterErrorNotifyCBR( P_TOYCAR_ERROR_NOTIFY_CBR pNotifyCBR, void *pUserInstance )
{
}

void CaptureManager_OnUnregisterErrorNotifyCBR( void )
{
}


void CaptureManager_OnOff( P_TOYCAR_COMPLETE_CBR pCompleteCBR, void *pUserInstance )
{
}

static void OnOff( void *pParameter )
{

}

void CaptureManager_OnIdle( P_TOYCAR_COMPLETE_CBR pCompleteCBR, void *pUserInstance )
{
 
}

static void OnIdle( void *pParameter )
{

}

void CaptureManager_OnS0( P_TOYCAR_COMPLETE_CBR pCompleteCBR, void *pUserInstance )
{

}

static void OnS0( void *pParameter )
{
 
}

void CaptureManager_OnS1( P_TOYCAR_COMPLETE_CBR pCompleteCBR, void *pUserInstance )
{
  
}

void CaptureManager_OnSysHalt( P_TOYCAR_COMPLETE_CBR pCompleteCBR, void *pUserInstance )
{
 
}

static void OnS1( void *pParameter )
{
 
}

// State handling.

void ToyCar_ChangeState( TOYCAR_STATE_ID NewID )
{
    assert_param( NewID < toyCarState_StateNumber ); // Invalid ID.
    assert_param( s_tToyCarStateTable[NewID].StateID == NewID );
    assert_param( s_tToyCarStateTable[s_CurrentToyCarState].StateID != NewID );

    TRACE( "[ToyCar] : %sState - OnStop\r\n", s_tToyCarStateTable[s_CurrentToyCarState].pStateName );
    (*s_tToyCarStateTable[s_CurrentToyCarState].pOnStop)();

    s_CurrentToyCarState = NewID;
    TRACE( "[ToyCar] : %sState - OnStart\r\n", s_tToyCarStateTable[s_CurrentToyCarState].pStateName );
    (*s_tToyCarStateTable[s_CurrentToyCarState].pOnStart)();
}

void ToyCarManager_RegisterState( TOYCAR_STATE_ID StateID, const char *pName, P_TOYCAR_ON_START_STATE pOnStart, P_TOYCAR_ON_STOP_STATE pOnStop, P_TOYCAR_ON_EVENT_HANDLER pOnEventHandler )
{
    assert_param( s_tToyCarStateTable[StateID].StateID == toyCarState_Invalid ); // Forbiddance of double registration.

    TRACE( "ToyCar_RegisterState %s\n", pName );
    s_tToyCarStateTable[StateID].StateID    = StateID;
    s_tToyCarStateTable[StateID].pStateName = pName;
    s_tToyCarStateTable[StateID].pOnStart   = pOnStart;
    s_tToyCarStateTable[StateID].pOnStop    = pOnStop;
    s_tToyCarStateTable[StateID].pOnEventHandler = pOnEventHandler;
}

// NEW MODE CBR

void CaptureManager_OnIdleStatusCBR( uint32_t Param, VOID *pUserInstance )
{

}



//  DEBUG

static void OnDisplayDebugInfo( char *szArguments )
{
    TRACE( "\t\t************************************\n" );
    TRACE( "\t\t********** ToyCar Info  ***********\n" );
    TRACE( "\t\t************************************\n" );
    //DEBUG_INFO( "CoachMode", CaptureDataStorage_GetCoachMode() );
    //DEBUG_INFO_STRING( "IsCoachModeBusy", ( ( CaptureDataStorage_IsCoachModeBusy() ) ? "YES" : "NO" ) );
    //DEBUG_INFO_STRING( "CurrentState", s_tToyCarStateTable[s_CurrentToyCarState].pStateName );

    //CaptureDataStorage_DisplayDebugInfo();
    //OnEventHandler( captureevent_DisplayDebugInfo, NULL, NULL, NULL );
}


