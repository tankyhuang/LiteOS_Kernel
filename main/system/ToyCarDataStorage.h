
#ifndef _CAPTURE_DATA_STORAGE_H_
#define _CAPTURE_DATA_STORAGE_H_

// MACRO
#define GPS_INFO_UPDATE_FREQ_IN_HZ                  3


// TYPE DEFINITIONS.

typedef void *HPC_OBSERVER;
typedef void *HFRAME_RATE_CREATOR;
typedef void (*PPC_UPDATE_CBR)( void *pUserInstance, uint16_t ProcessingCount );

typedef struct QUICK_VIEW_SETTING
{
	PBYTE pBuffer;
	UINT Width;
	UINT Height;
} QUICK_VIEW_SETTING, *PQUICK_VIEW_SETTING;

typedef enum BLACK_BELT_TYPE
{
	blackbelttype_TOP,
	blackbelttype_BOTTOM,

	blackbelttype_COUNT,
} BLACK_BELT_TYPE;

typedef enum VIEW_DISABLE_TYPE
{
	viewdisabletype_Disable,
	viewdisabletype_Enable,
	viewdisabletype_CountDown,

	viewdisabletype_Count
} VIEW_DISABLE_TYPE;

// FUNCTION DECLARATIONS.

void CaptureDataStorage_Initialization( void );

void CaptureDataStorage_RegisterStatusNotifyCBR( P_CAPTURE_STATUS_NOTIFY_CBR pStatusNotifyCBR, void *pUserInstance );
void CaptureDataStorage_UnregisterStatusNotifyCBR( void );
void CaptureDataStorage_NotifyStatus( CAPTURE_STATUS_ID StatusID, int32_t Status );

void CaptureDataStorage_RegisterErrorNotifyCBR( P_CAPTURE_ERROR_NOTIFY_CBR pErrorNotifyCBR, void *pUserInstance );
void CaptureDataStorage_UnregisterErrorNotifyCBR( void );
void CaptureDataStorage_NotifyError( CAPTURE_ERROR_ID ErrorID );

void CaptureDataStorage_RegisterCaptureCompleteCBR( P_CAPTURE_COMPLETE_CBR pCompleteCBR, void *pUserInstance );
void CaptureDataStorage_NotifyCaptureCompleteCBR( ERROR_T Result );

void CaptureDataStorage_TakeCaptureMutex( void );
void CaptureDataStorage_GiveCaptureMutex( void );

void CaptureDataStorage_SetCoachMode( I43_MODE Mode );
I43_MODE CaptureDataStorage_GetCoachMode( void );
bool CaptureDataStorage_IsCoachModeBusy( void );
void CaptureDataStorage_CompleteCoachModeTransition( I43_MODE Mode );

void CaptureDataStorage_SetHostMessageHandle( HHOSTMSG hHostMessageHandle );
HHOSTMSG CaptureDataStorage_GetHostMessageHandle( void );

void CaptureDataStorage_SetUserCallbackHandle( HUSERCALLBACK hUserCallbackHandle );
HUSERCALLBACK CaptureDataStorage_GetUserCallbackHandle( void );

void CaptureDataStorage_ReserveOff( bool Status );
void CaptureDataStorage_ReserveS0( bool Status );
void CaptureDataStorage_ReserveS1( bool Status );

bool CaptureDataStorage_IsOffReserved( void );
bool CaptureDataStorage_IsS0Reserved( void );
bool CaptureDataStorage_IsS1Reserved( void );

void CaptureDataStorage_SetCaptureReady( bool IsReady );
bool CaptureDataStorage_IsCaptureReady( void );

void CaptureDataStorage_SetCaptureInitiazation( bool Status );
bool CaptureDataStorage_IsCaptureInitialized( void );

void CaptureDataStorage_TakeImageInfoMutex( void );
void CaptureDataStorage_GiveImageInfoMutex( void );

void CaptureDataStorage_SetMountStatus( bool Status );
bool CaptureDataStorage_IsMount( void );

void CaptureDataStorage_SetIdleStatus( bool Status );
bool CaptureDataStorage_IsIdle( void );

void CaptureDataStorage_SetMpegFileClosed( bool Status );
bool CaptureDataStorage_IsMpegFileClosed( void );

void CaptureDataStorage_SetSensorShift( bool Status );
bool CaptureDataStorage_IsSensorShift( void );

void CaptureDataStorage_ReserveState( uint32_t CaptureState );
uint32_t CaptureDataStorage_GetReserveState( void );

void CaptureDataStorage_SetDataStamp( bool Status );
bool CaptureDataStorage_GetDataStamp( void );

void CaptureDataStorage_SetMotionDetection( bool Status );
bool CaptureDataStorage_GetMotionDetection( void );

void CaptureDataStorage_SetMovieTimeLimitation( uint32_t seconds );
uint32_t CaptureDataStorage_GetMovieTimeLimitation( void );

void CaptureDataStorage_CaptureProcessingStart( void );
void CaptureDataStorage_CaptureProcessingDone( void );
uint16_t CaptureDataStorage_GetCaptureProcessingCount( void );

void CaptureDataStorage_UpdateRemainCount( bool bUpdateAll );
void CaptureDataStorage_UpdateStillRemainCount( uint32_t RemainCount );
void CaptureDataStorage_UpdateMovieRemainTime( uint32_t RemainTime );
uint32_t CaptureDataStorage_GetStillRemainCount( void );
uint32_t CaptureDataStorage_GetMoiveRemainTime( void );

ERROR_T CaptureDataStorage_PreCheckMovieMediaSpace( void );

VOID CaptureDataStorage_ConstructMovieSkipAtom( PVOID pbybuffer, UINT puiSize );

VOID CaptureDataStorage_SetMovieStartFrameNum( UINT uiFrameNum );

ERROR_T CaptureDataStorage_MovieAtomContextIndexStart( VOID );
ERROR_T CaptureDataStorage_MovieAtomContextIndexPause( VOID );
ERROR_T CaptureDataStorage_MovieAtomContextIndexResume( VOID );
ERROR_T CaptureDataStorage_MovieAtomContextUpdate( PBYTE pbyData, PCHAR szDateTime );

void CaptureDataStorage_CreateMovieSkipAtom( void );
void CaptureDataStorage_DestroyMovieSkipAtom( void );

// FOR DEBUG
void CaptureDataStorage_DisplayDebugInfo( void );

#endif //_CAPTURE_DATA_STORAGE_H_

