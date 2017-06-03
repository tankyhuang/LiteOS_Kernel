#include "stm32f10x.h"
#include "types.h"
#include "debug.h"
#include "SystemMonitor.h"
#include "ToyCarManager.h"


//#define _IMAGE_INFO_DEBUG_

// MACRO

// TYPE DEFINITIONS

// FUNCTION DECLARATIONS
static void OnOff( P_TOYCAR_COMPLETE_CBR pCompleteCBR, void *pUserInstance );
static void OnIdle( P_TOYCAR_COMPLETE_CBR pCompleteCBR, void *pUserInstance );
static void OnS0( P_TOYCAR_COMPLETE_CBR pCompleteCBR, void *pUserInstance );
static void OnS1( P_TOYCAR_COMPLETE_CBR pCompleteCBR, void *pUserInstance );


// VARIABLE DEFINITIONS

// FUNCTION DEFINITIONS
static void OnOff( P_TOYCAR_COMPLETE_CBR pCompleteCBR, void *pUserInstance )
{
    TRACE( "[ToyCar] OnOff --- Can not accept\r\n" );
}

static void OnIdle( P_TOYCAR_COMPLETE_CBR pCompleteCBR, void *pUserInstance )
{
    TRACE( "[ToyCar] OnIdle --- Can not accept\r\n" );
}

static void OnS0( P_TOYCAR_COMPLETE_CBR pCompleteCBR, void *pUserInstance )
{
    TRACE( "[Capture] OnS0 --- Can not accept\r\n" );
}



ERROR_T ToyCarState_OnEventHandler( TOYCAR_EVENT_ID ID, void *pParam, P_TOYCAR_COMPLETE_CBR pCompleteCBR, void *pUserInstance )
{
    ERROR_T result = OK;

    switch( ID )
    {
        case toyCarState_Off:
            OnOff( pCompleteCBR, pUserInstance );
            break;
        case toyCarState_S0Start:
            OnIdle( pCompleteCBR, pUserInstance );
            break;

        default:
            result = ERROR;
            break;
    }
    return result;
}

// COMMON THREDS.


// LOCAL THREADS.


