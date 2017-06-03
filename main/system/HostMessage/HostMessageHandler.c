#include <stm32f10x.h>
#include <types.h>
#include <debug.h>
#include <host.h>
#include <HostMessageHandler.h>

typedef struct REGISTRY_INFORMATION
{
	HHOSTMSG    hHandle;
	P_HOST_MSG_CBR pHostMessageCBR;
	void *pUserInstance;
	HMESSAGEQUEUE hMessageQueue;
} REGISTRY_INFORMATION, *PREGISTRY_INFORMATION;


static const char       *ker_pHostSignature  = "Host";
static HOST_MSG_HANDLE *s_pHostMsgHandleList    = NULL;

#define HOST_MSG_ELEM(Name) static REGISTRY_INFORMATION s_sHostMsgRegistration_##Name = {0};
#include "HostMessageElement.inc"
#undef HOST_MSG_ELEM

#define NEW_MODE_ELEM(Mode) static REGISTRY_INFORMATION s_sHostMsgRegistration_##Mode = {0};
#include "NewModeElement.inc"
#undef NEW_MODE_ELEM


VOID HostMessageHandler_RegisterHostMessageCBR(HHOSTMSG hHandle, 
                                               uint32_t HostMessgaeID,
											   P_HOST_MSG_CBR pHostMessageCBR, 
											   VOID *pUserInstance)
{
	//TakeSemaphore(s_hHostMsgMutex);

	if(HostMessgaeID == HM_NEW_MODE)
	{
		ASSERT(FALSE);
	}
#define HOST_MSG_ELEM(Name)																			\
	else if(HostMessgaeID == HM_##Name)																\
	{																								\
		PREGISTRY_INFORMATION pinfo = (PREGISTRY_INFORMATION)&s_sHostMsgRegistration_##Name;	    \
																									\
		ASSERT(pinfo->pHostMessageCBR == NULL);												        \
		pinfo->hHandle          = hHandle;															\
		pinfo->pHostMessageCBR  = pHostMessageCBR;													\
		pinfo->pUserInstance    = pUserInstance;														\
		pinfo->hMessageQueue    = ((PHOST_MSG_HANDLE)hHandle)->hMessageQueue;							\
	}
#include "HostMessageElement.inc"
#undef HOST_MSG_ELEM
	else
	{
	    TRACE("Unknown HostMessgaeID = 0x%x\r\n",HostMessgaeID);
		ASSERT(FALSE);
	}

	//GiveSemaphore(s_hHostMsgMutex);
}

VOID HostMessageHandler_UnregisterHostMessageCBR(HHOSTMSG hHandle, uint32_t HostMessgaeID)
{
	//TakeSemaphore(s_hHostMsgMutex);

	if(HostMessgaeID == HM_NEW_MODE)
	{
		ASSERT(FALSE);
	}
#define HOST_MSG_ELEM(Name)																			\
	else if(HostMessgaeID == HM_##Name)																\
	{																								\
		PREGISTRY_INFORMATION pinfo = (PREGISTRY_INFORMATION)&s_sHostMsgRegistration_##Name;	    \
																									\
		ASSERT(pinfo != NULL);																		\
		ASSERT(pinfo->hHandle == hHandle);															\
        pinfo->hHandle      = NULL;                                                                 \
        pinfo->pHostMessageCBR = NULL;                                                              \
        pinfo->pUserInstance   = NULL;                                                              \
        pinfo->hMessageQueue   = NULL;                                                              \
    }
#include "HostMessageElement.inc"
#undef HOST_MSG_ELEM
	else
	{
		ASSERT(FALSE);
	}

	//GiveSemaphore(s_hHostMsgMutex);
}

void HostMessageHandler_Distributor(uint32_t MessageID, uint32_t MessageParam)
{
	//TakeSemaphore(s_hHostMsgMutex);

	if ( MessageID == HM_INVALID)
	{
	    ASSERT(FALSE);
	}
#define HOST_MSG_ELEM(Name)																					\
	else if(MessageID == HM_##Name)																			\
	{																										\
		PREGISTRY_INFORMATION pinfo = (PREGISTRY_INFORMATION)&s_sHostMsgRegistration_##Name;	            \
		if(pinfo->pHostMessageCBR != NULL)															        \
		{																									\
		    (*pinfo->pHostMessageCBR)(MessageParam, pinfo->pUserInstance);                                  \
		}																									\
	}
#include "HostMessageElement.inc"
#undef HOST_MSG_ELEM
	else
	{
        // Ignore it
        //fprintf( stderr, "Please add message 0x%x to HostMessageElement.inc\n", MessageID );
		//ASSERT(FALSE);
	}

	//GiveSemaphore(s_hHostMsgMutex);
}


HHOSTMSG HostMessageHandler_CreateHandle(PHOST_MSG_HANDLE handle, HTASK hTask, HMESSAGEQUEUE hMessageQueue)
{
	PHOST_MSG_HANDLE phandle = handle;

	//TakeSemaphore(s_hHostMsgMutex);

	phandle->pSignature = ker_pHostSignature;
	phandle->pNext = NULL;
	phandle->hTask = hTask;
	phandle->hMessageQueue = hMessageQueue;

	if(s_pHostMsgHandleList == NULL)
	{
		s_pHostMsgHandleList = phandle;
	}
	else
	{
		PHOST_MSG_HANDLE pcurrent = s_pHostMsgHandleList;

		while(pcurrent->pNext != NULL)
		{
			pcurrent = pcurrent->pNext;
		}
		pcurrent->pNext = phandle;
	}

	//GiveSemaphore(s_hHostMsgMutex);

	return ((HHOSTMSG)phandle);
}

