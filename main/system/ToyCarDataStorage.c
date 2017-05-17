
#include <assert.h>
#include <Timing.h>
#include <Trace/Trace.h>
#include <Hook/Hook.h>
#include <Api_def.h>
#include <I43_api.h>
#include <Storage/Media/Media.h>
#include <CommonType.h>
#include <CaptureParamType.h>
#include <CaptureParam.h>
#include <HostMessageHandler/HostMessageHandler.h>
#include <HostMessageHandler/UserCallbackDispatcher.h>
#include <ImageParameter/ImageSizeSpec.h>
#include <ImageParameter/ImageSetSensorWindow.h>
#include <CaptureManager/CaptureType.h>
#include <CaptureManager/CaptureDataStorage.h>
#include <SystemStatus.h>

// CONSTANT DEFINITIONS

static const int8_t *s_pSignature = "CaptureDataStorage";

#define	PREVIEW_FRAME_RATE_MAX  (uint16_t)30

#define MOV_USER_ATOM_MAX       128
#define MEDIA_SPACE_SIZE_FOR_EVENT      50      // percentage

enum
{
	PIP_PANORAMA_ASSIST_LAYER,
	PIP_BLACK_BELT_TOP,
	PIP_BLACK_BELT_BOTTOM,
	PIP_BLACK_SCREEN,
	PIP_QUICK_VIEW_LAYER,

	MAX_RAW_BUFFER_COUNT = 5,
};

// TYPE DEFINITIONS

typedef struct
{
    UINT        uiKeyFrameIndex;
    CHAR        szAtomContext[MOV_USER_ATOM_MAX];
} SMOVIE_ATOMINFO, *PSMOVIE_ATOMINFO;

// FUNCTION DECLARATIONS

static void OnUpdateProcessingCount( void );

// VARIABLE DEFINITIONS

static P_CAPTURE_STATUS_NOTIFY_CBR s_pStatusNotifyCBR   = NULL;
static void *s_pStatusNotifyUserInstance                = NULL;
static P_CAPTURE_ERROR_NOTIFY_CBR s_pErrorNotifyCBR     = NULL;
static void *s_pErrorNotifyUserInstance                 = NULL;
static P_CAPTURE_COMPLETE_CBR s_pCaptureCompleteCBR     = NULL;
static void *s_pCaptureCompleteUserInstance             = NULL;
static I43_MODE s_CurrentCoachMode                      = I43_IDLE_MODE;
static bool s_IsCoachModeBusy   = false;
static bool s_IsIdle            = false;
static bool s_IsCaptureReady    = false;
static bool s_IsInitialized     = false;
static bool s_IsMount           = false;
static bool s_IsS0Reserved      = false;
static bool s_IsS1Reserved      = false;
static bool s_IsCaptureBusy     = false;
static bool s_IsMpegFileClosed  = false;
static bool s_IsPasteDataStamp  = false;
static bool s_IsMotionDetection = false;
static bool s_IsSensorShift     = false;

static int16_t s_CaptureProcessingCount     = 0;
static int16_t s_ImagePreProcessingCount    = 0;
static int16_t s_ImageProcessingCount       = 0;
static uint32_t s_StillRemainCount          = -1;
static uint32_t s_MovieRemainTime           = -1;
static uint32_t s_MovieTimeLimitation 		= 0;
static uint32_t s_ReservedCaptureState      = -1;
static uint32_t s_MovieSkipAtomMaxEntry     = 0;
static uint32_t s_MoviePermitProtectMax     = 0;
static uint32_t s_MovieProtectFileNum       = 0;

static HHOSTMSG s_hHostMessageHandle        = NULL;
static HUSERCALLBACK s_hUserCallbackHandle  = NULL;

// Keep buffer index info
static uint32_t         uiAtomInfoUpdateIndex   = 0;
static uint32_t         uiAtomInfoRecordStart   = 0;
static uint32_t         uiAtomInfoRecordEnd     = 0;
static uint32_t         uiStartSensorFrame      = 0;

// Keep the start&end buffer index for current movie
static uint32_t         s_uiCurrentMovieSkipAtomIndexStart   = 0;
static uint32_t         s_uiCurrentMovieSkipAtomIndexEnd     = 0;

static HSEMAPHORE s_hCaptureManagerMutex    = NULL;

static bool s_IsCompleteCBRRegistered       = false;

static PSMOVIE_ATOMINFO s_pbyMovieAtomInfo  = NULL;


// FUNCTION DEFINITIONS

void CaptureDataStorage_Initialization( void )
{
	s_hCaptureManagerMutex = CreateMutex( "CaptureManager" );
}


static uint32_t GetMovieSkipAtomCacheSize( void )
{
	uint32_t time_limitation = GetStatus(CsCyclicSetting);
    uint32_t size;
    uint32_t entry;

	if ( time_limitation == cyclicmovie_Off )
	{
        // 45 min
        entry = ( time_limitation + 60 ) * GPS_INFO_UPDATE_FREQ_IN_HZ ;
	}
	else
	{
        entry = ( ( time_limitation * 3 ) * GPS_INFO_UPDATE_FREQ_IN_HZ );
	}

    size = sizeof( SMOVIE_ATOMINFO ) * entry;

    // Keep the max cache info
    s_MovieSkipAtomMaxEntry = entry;

    TRACE("SkipAtom cache size = %d, max entry = %d\n", size, s_MovieSkipAtomMaxEntry);

    return size;
}

void CaptureDataStorage_CreateMovieSkipAtom( void )
{
    UINT    uiAtomBufferSizeInByte;
    uiAtomBufferSizeInByte = GetMovieSkipAtomCacheSize();

    assert( NULL == s_pbyMovieAtomInfo );
    s_pbyMovieAtomInfo = (PSMOVIE_ATOMINFO)PoolAlloc( NULL,
                                                      uiAtomBufferSizeInByte,
                                                      POOL_MEMORY_CACHED );
    uiAtomInfoUpdateIndex = 0;
    uiAtomInfoRecordStart = 0;
    uiAtomInfoRecordEnd   = 0;
}

void CaptureDataStorage_DestroyMovieSkipAtom( void )
{
    if ( NULL != s_pbyMovieAtomInfo )
    {
        PoolFree( NULL, s_pbyMovieAtomInfo );
        s_pbyMovieAtomInfo = NULL;
    }

    uiAtomInfoUpdateIndex = 0;
    uiAtomInfoRecordStart = 0;
    uiAtomInfoRecordEnd   = 0;
    s_MovieSkipAtomMaxEntry = 0;
}

void CaptureDataStorage_RegisterStatusNotifyCBR( P_CAPTURE_STATUS_NOTIFY_CBR pStatusNotifyCBR, void *pUserInstance )
{
	assert( s_pStatusNotifyCBR == NULL );

	s_pStatusNotifyCBR = pStatusNotifyCBR;
	s_pStatusNotifyUserInstance = pUserInstance;
}

void CaptureDataStorage_UnregisterStatusNotifyCBR( void )
{
	s_pStatusNotifyCBR = NULL;
	s_pStatusNotifyUserInstance = NULL;
}

void CaptureDataStorage_NotifyStatus( CAPTURE_STATUS_ID StatusID, int32_t Status )
{
	P_CAPTURE_STATUS_NOTIFY_CBR pcbr = s_pStatusNotifyCBR;
	void *pinstance = s_pStatusNotifyUserInstance;

	if( pcbr != NULL )
	{
		TRACE_D( "Notify Capture status (%d, %d)\r\n", StatusID, Status );
		(*pcbr)( pinstance, StatusID, Status );
	}
}

void CaptureDataStorage_RegisterErrorNotifyCBR( P_CAPTURE_ERROR_NOTIFY_CBR pErrorNotifyCBR, void *pUserInstance )
{
	assert( s_pErrorNotifyCBR == NULL );

	s_pErrorNotifyCBR = pErrorNotifyCBR;
	s_pErrorNotifyUserInstance = pUserInstance;
}

void CaptureDataStorage_UnregisterErrorNotifyCBR( void )
{
	s_pErrorNotifyCBR = NULL;
	s_pErrorNotifyUserInstance = NULL;
}

void CaptureDataStorage_NotifyError( CAPTURE_ERROR_ID ErrorID )
{
	if( s_pErrorNotifyCBR != NULL )
	{
		TRACE( "[Capture] : Error was occurred (%d)\r\n", ErrorID );
		(*s_pErrorNotifyCBR)( s_pErrorNotifyUserInstance, ErrorID );
	}
}

void CaptureDataStorage_RegisterCaptureCompleteCBR( P_CAPTURE_COMPLETE_CBR pCompleteCBR, void *pUserInstance )
{
	assert( s_pCaptureCompleteCBR == NULL );

	s_IsCompleteCBRRegistered = true;
	s_pCaptureCompleteCBR = pCompleteCBR;
	s_pCaptureCompleteUserInstance = pUserInstance;
}

void CaptureDataStorage_NotifyCaptureCompleteCBR( ERROR_T Result )
{
	//assert( s_IsCompleteCBRRegistered );

	s_IsCompleteCBRRegistered = false;
	if( s_pCaptureCompleteCBR != NULL )
	{
		P_CAPTURE_COMPLETE_CBR pcbr = s_pCaptureCompleteCBR;
		void *pinstance = s_pCaptureCompleteUserInstance;

		s_pCaptureCompleteCBR = NULL;
		s_pCaptureCompleteUserInstance = NULL;

		TRACE( "[Capture] : CompleteCBR\r\n" );
		(*pcbr)( pinstance, Result );
	}
}



void CaptureDataStorage_TakeCaptureMutex( void )
{
}

void CaptureDataStorage_GiveCaptureMutex( void )
{
}

void CaptureDataStorage_ReserveS0( bool Status )
{
	s_IsS0Reserved = Status;	// true : reserved, false : not reserved.
}

bool CaptureDataStorage_IsS0Reserved( void )
{
	return s_IsS0Reserved;
}

void CaptureDataStorage_ReserveS1( bool Status )
{
	s_IsS1Reserved = Status;	// true : reserved, false : not reserved.
}

bool CaptureDataStorage_IsS1Reserved( void )
{
	return s_IsS1Reserved;
}

void CaptureDataStorage_SetCoachMode( I43_MODE Mode )
{
	assert( !s_IsCoachModeBusy );

	s_IsCoachModeBusy = true;
	s_CurrentCoachMode = Mode;

	TRACE_D( "[Capture] : Set coach mode (0x%x)\r\n", Mode );
	CaptureDataStorage_GiveCaptureMutex();
	I43_SetMode( Mode );
	CaptureDataStorage_TakeCaptureMutex();
}

I43_MODE CaptureDataStorage_GetCoachMode( void )
{
	return s_CurrentCoachMode;
}

bool CaptureDataStorage_IsCoachModeBusy( void )
{
	return s_IsCoachModeBusy;
}

void CaptureDataStorage_SetIdleStatus( bool Status )
{
	s_IsIdle = Status;
}
bool CaptureDataStorage_IsIdle( void )
{
	return s_IsIdle;
}

void CaptureDataStorage_SetMpegFileClosed( bool Status )
{
	s_IsMpegFileClosed = Status;
}

bool CaptureDataStorage_IsMpegFileClosed( void )
{
	return s_IsMpegFileClosed;
}

void CaptureDataStorage_SetSensorShift( bool Status )
{
	s_IsSensorShift = Status;
}

bool CaptureDataStorage_IsSensorShift( void )
{
	return s_IsSensorShift;
}

void CaptureDataStorage_ReserveState( uint32_t CaptureState )
{
	s_ReservedCaptureState = CaptureState;
}

uint32_t CaptureDataStorage_GetReserveState( void )
{
	return s_ReservedCaptureState;
}

void CaptureDataStorage_SetDataStamp( bool Status )
{
	s_IsPasteDataStamp = Status;
}

bool CaptureDataStorage_GetDataStamp( void )
{
	return s_IsPasteDataStamp;
}

void CaptureDataStorage_SetMotionDetection( bool Status )
{
	s_IsMotionDetection = Status;
}

bool CaptureDataStorage_GetMotionDetection( void )
{
	return s_IsMotionDetection;
}

void CaptureDataStorage_SetMovieTimeLimitation( uint32_t seconds )
{
	TRACE("SetMovieTimeLimitation %d\n", seconds);
	s_MovieTimeLimitation = seconds;
}

/*
    %func
        CaptureDataStorage_GetMovieTimeLimitation
    %desc

    %return
        Movie time in seconds
/**/

uint32_t CaptureDataStorage_GetMovieTimeLimitation( void )
{
	return s_MovieTimeLimitation;
}

void CaptureDataStorage_CompleteCoachModeTransition( I43_MODE Mode )
{
	s_CurrentCoachMode = Mode;
	s_IsCoachModeBusy = false;
	TRACE_D( "[Capture] : Complete coach mode transition (0x%x)\r\n", Mode );
}

void CaptureDataStorage_SetHostMessageHandle( HHOSTMSG hHostMessageHandle )
{
	s_hHostMessageHandle = hHostMessageHandle;
}

HHOSTMSG CaptureDataStorage_GetHostMessageHandle( void )
{
	return s_hHostMessageHandle;
}

void CaptureDataStorage_SetUserCallbackHandle( HUSERCALLBACK hUserCallbackHandle )
{
	s_hUserCallbackHandle = hUserCallbackHandle;
}

HUSERCALLBACK CaptureDataStorage_GetUserCallbackHandle( void )
{
	return s_hUserCallbackHandle;
}

void CaptureDataStorage_SetCaptureInitiazation( bool Status )
{
}

bool CaptureDataStorage_IsCaptureInitialized( void )
{
    return false;
}

void CaptureDataStorage_SetCaptureReady( bool IsReady )
{
	s_IsCaptureReady = IsReady;
}

bool CaptureDataStorage_IsCaptureReady( void )
{
	return s_IsCaptureReady;
}

void CaptureDataStorage_SetMountStatus( bool Status )
{
	s_IsMount = Status;
}

bool CaptureDataStorage_IsMount( void )
{
	return s_IsMount;
}

static void OnUpdateProcessingCount( void )
{
	CAPTURE_MODE mode = CaptureParam_GetParam( CAPTURE_PARAM( CaptureMode ) );
	bool IsBusy       = FALSE;

	if ( mode == capturemode_Movie1080 || mode == capturemode_Movie )
	{
		return ;
	}
	else if ( mode == capturemode_StillSingle )
	{
		IsBusy = (s_ImageProcessingCount >= MAX_RAW_BUFFER_COUNT ) ? true : false;
	}

	if ( s_IsCaptureBusy != IsBusy )
	{
		s_IsCaptureBusy = IsBusy;
		CaptureDataStorage_NotifyStatus( capturestatusid_CaptureBusy, (int32_t)IsBusy );
	}
}

void CaptureDataStorage_CaptureProcessingStart( void )
{
	s_CaptureProcessingCount++;

	assert( s_ImageProcessingCount <= MAX_RAW_BUFFER_COUNT );
	OnUpdateProcessingCount();

	//s_StillRemainCount--;
	//CaptureDataStorage_NotifyStatus( capturestatusid_StillRemainCount, s_StillRemainCount );
	CaptureDataStorage_NotifyStatus( capturestatusid_UpdateProcessingCount, s_CaptureProcessingCount );
}

void CaptureDataStorage_CaptureProcessingDone( void )
{
    // For TI5150 , It's draft capture mode
    // No raw data grab

    if ( s_CaptureProcessingCount > 0 )
    {
        s_CaptureProcessingCount--;
        assert( s_CaptureProcessingCount >= 0 );

        CaptureDataStorage_NotifyStatus( capturestatusid_UpdateProcessingCount, s_CaptureProcessingCount );

        //OnUpdateProcessingCount();
        TRACE_D( "[Capture] : ProcessingDone %d\r", s_CaptureProcessingCount  );
    }

	if( s_CaptureProcessingCount == 0 )
	{
		CaptureDataStorage_UpdateRemainCount( FALSE );
	}
}

uint16_t CaptureDataStorage_GetCaptureProcessingCount( void )
{
	return s_CaptureProcessingCount;
}

void CaptureDataStorage_UpdateStillRemainCount( uint32_t RemainCount )
{
	assert( RemainCount >= 0);

	s_StillRemainCount = RemainCount;
	CaptureDataStorage_NotifyStatus( capturestatusid_StillRemainCount, RemainCount );
}

void CaptureDataStorage_UpdateMovieRemainTime( uint32_t RemainTime )
{
	assert( RemainTime >= 0);

	s_MovieRemainTime = RemainTime;
	CaptureDataStorage_NotifyStatus( capturestatusid_MovieRemainTime, RemainTime );
}

uint32_t CaptureDataStorage_GetStillRemainCount( void )
{
	return s_StillRemainCount;
}

uint32_t CaptureDataStorage_GetMoiveRemainTime( void )
{
	return s_MovieRemainTime;
}

uint32_t GetMovieCompressRatio( MOVIE_RESOLUTION_MODE movie_resolution )
{
	switch ( movie_resolution )
	{
		case movieresolutionmode_WVGA30fps   :	return MOVIE_RECORD_CMP_RATIO_480P_30FPS;
		case movieresolutionmode_WVGA60fps   :  return MOVIE_RECORD_CMP_RATIO_480P_60FPS;
		case movieresolutionmode_HD30fps	 :	return MOVIE_RECORD_CMP_RATIO_720P_30FPS;
		case movieresolutionmode_HD60fps	 :  return MOVIE_RECORD_CMP_RATIO_720P_60FPS;
		case movieresolutionmode_FHD30fps    :  return MOVIE_RECORD_CMP_RATIO_1080P;

		default								 :  return MOVIE_RECORD_CMP_RATIO_1080P;
	}

	return MOVIE_RECORD_CMP_RATIO_1080P;
}

void CaptureDataStorage_UpdateRemainCount( bool bUpdateAll )
{
    uint32_t ImageCount  = 0;
	uint32_t Remaintime  = 0;
	uint32_t Remaincount = 0;
	uint32_t image_width;
	uint32_t image_height;
	uint32_t movie_fps;
	uint32_t movie_Cr;

    int32_t     movie_resolution ;
	int32_t     still_resolution = GetStatus( CsStillResolution );
    int32_t     capture_mode = CaptureParam_GetParam( CAPTURE_PARAM(CaptureMode) );

    if ( capture_mode == capturemode_Movie1080 )
    {
        movie_resolution = GetStatus( CsMovieResolution );
    }
    else
    {
        movie_resolution = GetStatus( CsMovieResolution );
    }


	image_width  = Image_GetMovieImageSizeWidth(movie_resolution);
	image_height = Image_GetMovieImageSizeHeight(movie_resolution);
	movie_fps    = ImageMovie_GetSensorFrameRateEx(movie_resolution);
	movie_Cr     = GetMovieCompressRatio(movie_resolution);

    if ( capture_mode == capturemode_Unknown || bUpdateAll )
    {
		TRACE_D( "[Capture] : unknown mode\r" );

        Media_GetMovieRemainTime( image_width, image_height, movie_fps, movie_Cr, &Remaintime, &ImageCount );
        Media_GetStillRemainImage( still_resolution, &Remaincount, &ImageCount );
        CaptureDataStorage_UpdateMovieRemainTime( Remaintime );
		CaptureDataStorage_UpdateStillRemainCount( Remaincount );
    }
    else if( (capture_mode == capturemode_Movie) || (capture_mode == capturemode_Movie1080) )
	{
		STORAGE_LOCATION_TYPE storagetype	  = (STORAGE_LOCATION_TYPE)CaptureParam_GetParam( CAPTURE_PARAM( StorageLocationType ) );

        Media_GetMovieRemainTime( image_width, image_height, movie_fps, movie_Cr, &Remaintime, &ImageCount );

		TRACE_D( "[Capture] : Movie Remain Time = %ds\r\n", Remaintime );

		CaptureDataStorage_UpdateMovieRemainTime( Remaintime );
	}
	else
	{
		STILL_RESOLUTION_MODE stillresolution = (STILL_RESOLUTION_MODE)CaptureParam_GetParam( CAPTURE_PARAM( StillResolutionMode ) );

        Media_GetStillRemainImage( still_resolution, &Remaincount, &ImageCount );

		CaptureDataStorage_UpdateStillRemainCount( Remaincount );
		TRACE_D( "[Capture] : Still Remain Count = %d\r\n", Remaincount );
	}

	CaptureDataStorage_NotifyStatus( capturestatusid_ImageCount, ImageCount );
}

ERROR_T CaptureDataStorage_PreCheckMovieMediaSpace( void )
{
	ERROR_T result = OK;
    uint32_t ImageCount  = 0;
	uint32_t Remaintime  = 0;
	uint32_t image_width;
	uint32_t image_height;
	uint32_t movie_fps;
	uint32_t movie_Cr;
	uint32_t secondsRequire;
    uint32_t mediaSpaceInM;
    uint32_t clip_time;
    uint32_t clip_maxsizeInM;

    if ( CaptureDataStorage_IsMount() == false )
    {
        return OK;
    }

    // Unnecessary to check internal
    if ( GetStatus( CsCyclicSetting) == storagelocationtype_Internal )
    {
        return OK;
    }

    int32_t     movie_resolution = capturemode_Movie1080;
    int32_t     capture_mode     = CaptureParam_GetParam( CAPTURE_PARAM(CaptureMode) );

    clip_time = GetStatus( CsCyclicSetting);

    if ( cyclicmovie_Off == clip_time )
    {
        TRACE( "[Capture] : PreCheckMovieMediaSpace Abort!!\r\n" );
        return OK;
    }

	// Double space by setting to make sure media space is enough
	secondsRequire = clip_time * 2;

	TRACE( "[Capture] : PreCheckMovieMediaSpace require %dsec\r\n", secondsRequire );

    mediaSpaceInM = Media_GetMediaSpace( );
    switch ( clip_time )
    {
        case cyclicmovie_1min : clip_maxsizeInM = 85;  break;
        case cyclicmovie_2min : clip_maxsizeInM = 170; break;
        case cyclicmovie_3min : clip_maxsizeInM = 255; break;
        case cyclicmovie_5min : clip_maxsizeInM = 425; break;

        default : clip_maxsizeInM = 170; break;
    }

    s_MoviePermitProtectMax = mediaSpaceInM * MEDIA_SPACE_SIZE_FOR_EVENT / (100 * clip_maxsizeInM );
    s_MovieProtectFileNum   = Meida_GetProtectFileNum();

	TRACE( "Check lock files %d-%d\n", s_MovieProtectFileNum, s_MoviePermitProtectMax );

    if ( capture_mode == capturemode_Movie1080 )
    {
        movie_resolution = GetStatus( CsMovieResolution );
    }
    else if ( capture_mode == capturemode_Movie )
    {
        movie_resolution = GetStatus( CsMovieResolution );
    }

	image_width  = Image_GetMovieImageSizeWidth(movie_resolution);
	image_height = Image_GetMovieImageSizeHeight(movie_resolution);
	movie_fps    = ImageMovie_GetSensorFrameRateEx(movie_resolution);
	movie_Cr     = GetMovieCompressRatio(movie_resolution);

    Media_GetMovieRemainTime( image_width, image_height, movie_fps, movie_Cr, &Remaintime, &ImageCount );

	TimingMarkF(TIMING_GROUP_TEMP, "[CHK-Media] - S" );

    // Check Protect file
    while( s_MovieProtectFileNum > s_MoviePermitProtectMax )
    {
        uint16_t indexStart   = 1;
        uint16_t indexCurrent = 1;
        uint16_t result;

        if ( I43ERR_OK != I43_GetDCFOldestLockIndex( indexStart, &indexCurrent ) )
        {
            //
            CAUTION( "No locked file! \n" );
            break;
        }

        I43_SetCurrentImage( indexCurrent );

        // Unlock it , make it can be delete
        I43_FileOperation( 0,
                           I43_SCOPE_DCF_OBJECT,
                           I43_FILE_OP_UNLOCK,
                           &result );

        if ( indexStart == ImageCount )
        {
            break;
        }

        if ( OK != result )
        {
            indexStart = indexCurrent + 1;
        }

        s_MovieProtectFileNum   = Meida_GetProtectFileNum();
    }

    // Check free space for cyclic
	while( Remaintime < secondsRequire )
	{
		result = Media_ReleaseFileSpace();
		Media_GetMovieRemainTime( image_width, image_height, movie_fps, movie_Cr, &Remaintime, &ImageCount );
		if ( OK != result )
		{
			TimingMarkF(TIMING_GROUP_TEMP, "[CHK-Media] - Abort" );
			break;
		}
	}

    // Restore to the last image index
    I43_SetCurrentImage( ImageCount );

	TimingMarkF(TIMING_GROUP_TEMP, "[CHK-Media] - E" );

	TRACE( "[Capture] : Free Space for Movie = %dmin %dsecnds\n\r", Remaintime/60, Remaintime%60 );

	// During mpeg seamless recording, this msg is unexpcted
	//CaptureDataStorage_UpdateMovieRemainTime( Remaintime );

	return result;
}

ERROR_T CaptureDataStorage_MovieAtomContextIndexStart( VOID )
{
    // Used for mov skip atom data
    uiStartSensorFrame = SensorGetCurrentFrame();

    uiAtomInfoRecordStart = uiAtomInfoUpdateIndex;

    return OK;
}

ERROR_T
CaptureDataStorage_MovieAtomContextIndexPause( VOID )
{
    uiAtomInfoRecordEnd = uiAtomInfoUpdateIndex;
    s_uiCurrentMovieSkipAtomIndexEnd   = uiAtomInfoRecordEnd;
    s_uiCurrentMovieSkipAtomIndexStart = uiAtomInfoRecordStart;

    TRACE( "OnVideoEnd index %d\n", uiAtomInfoRecordEnd );
    return OK;
}

ERROR_T
CaptureDataStorage_MovieAtomContextIndexResume( VOID )
{
    uint32_t overlap_entries = 0;

    switch ( GetStatus( CsFileOverlapSetting ) )
    {
    case fileoverlapsetting_0sec  :
            if ( (uiAtomInfoRecordEnd + 1) < (s_MovieSkipAtomMaxEntry - 1) )
            {
                uiAtomInfoRecordStart = uiAtomInfoRecordEnd + 1;
            }
            else
            {
                uiAtomInfoRecordStart = 0;
            }
            break;

    case fileoverlapsetting_1sec  :
            overlap_entries = GPS_INFO_UPDATE_FREQ_IN_HZ * 1 ;
            if ( (uiAtomInfoRecordEnd -  overlap_entries) >= 0 )
            {
                uiAtomInfoRecordStart = uiAtomInfoRecordEnd - overlap_entries;
            }
            else
            {
                uiAtomInfoRecordStart = (s_MovieSkipAtomMaxEntry - 1 ) - (overlap_entries - uiAtomInfoRecordEnd );
            }

            break;

    case fileoverlapsetting_2sec  :
        overlap_entries = GPS_INFO_UPDATE_FREQ_IN_HZ * 2 ;
        if ( (uiAtomInfoRecordEnd -  overlap_entries) >= 0 )
        {
            uiAtomInfoRecordStart = uiAtomInfoRecordEnd - overlap_entries;
        }
        else
        {
            uiAtomInfoRecordStart = (s_MovieSkipAtomMaxEntry - 1 ) - (overlap_entries - uiAtomInfoRecordEnd );
        }

        break;

    default :   uiAtomInfoRecordStart = uiAtomInfoRecordEnd + 1;
    }

    uiStartSensorFrame = SensorGetCurrentFrame();

    TRACE( "OnVideoStart index %d\n", uiAtomInfoRecordStart );

    return OK;

}

ERROR_T
CaptureDataStorage_MovieAtomContextUpdate( PBYTE pbyData, PCHAR szDateTime )
{
    STATIC UINT            uiWriteFreqControl = 0;
    PSMOVIE_ATOMINFO       psAtomInfo;

    if ( s_pbyMovieAtomInfo == NULL )
    {
        FATAL_ERROR("Movie Atom buffer is not ready\n");
        return ERROR;
    }

    if ( uiAtomInfoUpdateIndex >= s_MovieSkipAtomMaxEntry )
    {
        uiAtomInfoUpdateIndex = 0;
    }

    if ( ( NULL == pbyData) || (NULL == szDateTime) )
    {
        return ERROR;
    }

    psAtomInfo = s_pbyMovieAtomInfo + uiAtomInfoUpdateIndex;

    // Set the GPS info and key frame index belongs to
    psAtomInfo->uiKeyFrameIndex = SensorGetCurrentFrame() - uiStartSensorFrame;

    memset( psAtomInfo->szAtomContext, 0, sizeof( CHAR ) * MOV_USER_ATOM_MAX );
    sprintf( psAtomInfo->szAtomContext, "%s %s\n", szDateTime, pbyData);

    if ( uiAtomInfoUpdateIndex < ( s_MovieSkipAtomMaxEntry - 1 ) )
    {
        uiAtomInfoUpdateIndex++;
    }
    else
    {
        uiAtomInfoUpdateIndex = 0;
    }

    return  OK;
}

VOID CaptureDataStorage_SetMovieStartFrameNum( UINT uiFrameNum )
{
    uiStartSensorFrame = uiFrameNum;
}

VOID
CaptureDataStorage_ConstructMovieSkipAtom( PVOID   pvUserDataAddr,
                                           UINT    puiUserDataSize )
{
    UINT    uiGpsEntries;
    UINT    uiDataSize;
    PBYTE   pbySkipAtom;
    PUINT   puiGpsEntries;
    PBYTE   pbyGpsData;
    UINT    uiStart;
    UINT    uiEnd;

    uiStart = s_uiCurrentMovieSkipAtomIndexStart;
    uiEnd   = s_uiCurrentMovieSkipAtomIndexEnd;

    TRACE("Start - End = %d , %d\n", uiStart, uiEnd );

    if ( NULL == s_pbyMovieAtomInfo )
    {
        FATAL_ERROR("Movie Atom buffer is not ready\n");
        return;
    }

    if ( uiEnd == uiStart )
    {
        // Abort immediately in case of no GPS data recorded.
        return;
    }
    else if ( uiEnd > uiStart )
    {
        uiGpsEntries = uiEnd - uiStart;
    }
    else
    {
        uiGpsEntries = uiEnd + s_MovieSkipAtomMaxEntry - 1 - uiStart;
    }

    uiDataSize = 16 +   /* Skip atom identifier */
                 4  +   /* User defined entries */
                 sizeof( SMOVIE_ATOMINFO ) * uiGpsEntries;

    // Buffer freed by lower layer movie encoder
    pbySkipAtom = PoolAlloc( NULL,
                             uiDataSize,
                             POOL_MEMORY_CACHED );
    if ( NULL == pbySkipAtom )
    {
        // Abort
        FATAL_ERROR("Movie skip atom allocate memory fail\n");
        return ;
    }

    puiGpsEntries = (PUINT)( pbySkipAtom + 16 );
    pbyGpsData    = pbySkipAtom + 16 + 4;

    memcpy( pbySkipAtom, "GPS INFORMATION", 16 );
    *puiGpsEntries = uiGpsEntries;

    if ( uiEnd > uiStart )
    {
        memcpy( pbyGpsData, (PBYTE)(s_pbyMovieAtomInfo + uiStart), sizeof( SMOVIE_ATOMINFO ) * uiGpsEntries );
    }
    else
    {
        UINT    uiTrailerSizeInByte = sizeof( SMOVIE_ATOMINFO ) * (s_MovieSkipAtomMaxEntry - 1 - uiStart);

        memcpy( pbyGpsData, (PBYTE)(s_pbyMovieAtomInfo + uiStart), uiTrailerSizeInByte );
        memcpy( pbyGpsData+uiTrailerSizeInByte, (PBYTE)(s_pbyMovieAtomInfo), sizeof( SMOVIE_ATOMINFO ) * uiGpsEntries  - uiTrailerSizeInByte );
    }

    TRACE( GREEN("skipatom = 0x%x, items = %d totalsize = %d") "\n", pbySkipAtom, uiGpsEntries, uiDataSize );

    *((PBYTE*)pvUserDataAddr)  = (PBYTE)pbySkipAtom;
    *((UINT *)puiUserDataSize) = uiDataSize;
}

// FOR DEBUG.

void CaptureDataStorage_DisplayDebugInfo( void )
{
    TRACE( "\t\t************************************\n" );
    TRACE( "\t\t********* CaptureDataStorage  ******\n" );
    TRACE( "\t\t************************************\n" );

	DEBUG_INFO_STRING( "IsCaptureCompleted", ( ( s_pCaptureCompleteCBR == NULL ) ? "YES" : "NO" ) );
	DEBUG_INFO("s_IsCoachModeBusy ",       s_IsCoachModeBusy  );
	DEBUG_INFO("s_IsIdle          ",       s_IsIdle           );
	DEBUG_INFO("s_IsCaptureReady  ",       s_IsCaptureReady   );
	DEBUG_INFO("s_IsInitialized   ",       s_IsInitialized    );
	DEBUG_INFO("s_IsMount         ",       s_IsMount          );
	DEBUG_INFO("s_IsS0Reserved    ",       s_IsS0Reserved     );
	DEBUG_INFO("s_IsS1Reserved    ",       s_IsS1Reserved     );
	DEBUG_INFO("s_IsCaptureBusy   ",       s_IsCaptureBusy    );
	DEBUG_INFO("s_IsMpegFileClosed",       s_IsMpegFileClosed );
	DEBUG_INFO("s_IsSensorShift",          s_IsSensorShift    );
}


