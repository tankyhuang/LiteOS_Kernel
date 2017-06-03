

#ifndef _HOOK_H_
#define _HOOK_H_

#include <Types.h>

#ifdef __cplusplus
extern "C"
{
#endif


//	CONSTANT DEFINITIONS


//	TYPE DEFINITIONS

typedef void *HHOOK;
typedef void *HHOOKPROC;

typedef void (*PHOOKPROC)(void *pUserInstance, ...);


//	FUNCTION DECLARATIONS

HHOOK		CreateHook(void);
void		DestroyHook(HHOOK hHook);
HHOOKPROC	AddHookProcedure(HHOOK hHook, PHOOKPROC pProcedure, void *pUserInstance);
void		RemoveHookProcedure(HHOOK hHook, HHOOKPROC hHookProc);
void		ExecuteHookProcedure(HHOOK hHook, ...);


#ifdef __cplusplus
}
#endif

#endif
