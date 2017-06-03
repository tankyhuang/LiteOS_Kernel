
#ifndef _SYSTEM_MONITOR_H_
#define _SYSTEM_MONITOR_H_

// TYPE DEFINITIONS
typedef enum _SYS_RED_RAY_STATUS
{
    sysRedRayStatus_Unknown = -1,
    sysRedRayStatus_None  = 0,
	sysRedRayStatus_L_True,
	sysRedRayStatus_R_True,
    sysRedRayStatus_L_R_True,
    
	SYS_RED_RAY_STATUS_COUNT
} SYS_RED_RAY_STATUS;

typedef void *HSYSM_RED_RAY_STATUS_CBR;
typedef void (*PSYSM_RED_RAY_STATUS_CBR)(void* pUserInstance, SYS_RED_RAY_STATUS status);

// FUNCTION DECLARATIONS

void 
SYSTEM_MONITOR_Init( void );


#endif //_SYSTEM_MONITOR_H_

