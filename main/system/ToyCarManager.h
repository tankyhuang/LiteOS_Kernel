#ifndef _STATE_MANAGER_H__
#define _STATE_MANAGER_H__


// TYPE DEFINITIONS
typedef enum _TOYCAR_EVENT_ID
{
	toyCarEvent_Off,
	toyCarEvent_Idle,
	toyCarEvent_S0,
	toyCarEvent_S1,
    toyCarEvent_RedRay,
    toyCarEvent_OnCreate
} TOYCAR_EVENT_ID;

typedef enum _TOYCAR_STATE_ID
{
	toyCarState_Invalid = -1,
	toyCarState_Off,
    toyCarState_Avoidence,
    toyCarState_Charge,
	toyCarState_S0Start,

	toyCarState_StateNumber,
} TOYCAR_STATE_ID;

typedef enum TOYCAR_ERROR_ID
{
    toyCarError_NocardError,
	toyCarError_AstarError,
    toyCarError_NoMediaSpace,

	toyCarError_ErrorIDCount
} TOYCAR_ERROR_ID;



typedef void (*P_TOYCAR_COMPLETE_CBR)( void *pUserInstance, ERROR_T Result );
typedef void (*P_TOYCAR_STATUS_NOTIFY_CBR)( void *pUserInstance, TOYCAR_STATE_ID StatusID, int32_t Status );
typedef void (*P_TOYCAR_ERROR_NOTIFY_CBR)( void *pUserInstance, TOYCAR_ERROR_ID ID );

typedef void (*P_TOYCAR_ON_START_STATE)( void );
typedef void (*P_TOYCAR_ON_STOP_STATE)( void );
typedef ERROR_T (*P_TOYCAR_ON_EVENT_HANDLER)( TOYCAR_EVENT_ID ID,
                                               void *pParam,
                                               P_TOYCAR_COMPLETE_CBR pCompleteCBR,
                                               void *pUserInstance );
                                               
typedef struct STOYCAR_STATE
{
	const char *pStateName;
	TOYCAR_STATE_ID StateID;
	P_TOYCAR_ON_START_STATE pOnStart;
	P_TOYCAR_ON_STOP_STATE pOnStop;
	P_TOYCAR_ON_EVENT_HANDLER pOnEventHandler;
}STOYCAR_STATE, *PSTOYCAR_STATE;

// FUNCTION DECLARATIONS.

void ToyCarManager_Init( void );
void ToyCarManager_OnEventHandler( TOYCAR_EVENT_ID EventID, void *pParam, P_TOYCAR_COMPLETE_CBR pCompleteCBR, void *pUserInstance );

void _ToyCar_OnRedRayNotificationCBR( void *pUserInstance, SYS_RED_RAY_STATUS status );

void ToyCarManager_OnRegisterStatusNotifyCBR( P_TOYCAR_STATUS_NOTIFY_CBR pStatusNotifyCBR, void *pUserInstance );
void ToyCarManager_OnUnregisterStatusNotifyCBR( void );
void ToyCarManager_OnRegisterErrorNotifyCBR( P_TOYCAR_ERROR_NOTIFY_CBR pNotifyCBR, void *pUserInstance );
void ToyCarManager_OnUnregisterErrorNotifyCBR( void );

void ToyCarManager_OnOff( P_TOYCAR_COMPLETE_CBR pCompleteCBR, void *pUserInstance );
void ToyCarManager_OnIdle( P_TOYCAR_COMPLETE_CBR pCompleteCBR, void *pUserInstance );
void ToyCarManager_OnS0( P_TOYCAR_COMPLETE_CBR pCompleteCBR, void *pUserInstance );
void ToyCarManager_OnS1( P_TOYCAR_COMPLETE_CBR pCompleteCBR, void *pUserInstance );

void _ToyCar_OnCreated( uint32_t Param, void *pUserInstance );

void ToyCarManager_OnSysHalt( P_TOYCAR_COMPLETE_CBR pCompleteCBR, void *pUserInstance );

void ToyCar_ChangeState( TOYCAR_STATE_ID NewID );
void ToyCarManager_RegisterState( TOYCAR_STATE_ID ID, const char *pName, P_TOYCAR_ON_START_STATE pOnStart, P_TOYCAR_ON_STOP_STATE pOnStop, P_TOYCAR_ON_EVENT_HANDLER pOnEventHandler );


#endif //_STATE_MANAGER_H__
