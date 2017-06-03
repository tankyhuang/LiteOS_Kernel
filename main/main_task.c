#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <debug.h>
#include <console.h>
#include "los_task.h"
#include "los_queue.h"
#include "los_membox.h"
#include "HostMessageHandler.h"
#include "main_task.h"

static UINT32 m_mainTaskId = 0;
static UINT32 m_mainQ      = 0;

static const char		*ker_pMsgSignature	= "Msg";
static const char       *ker_pCbrSignature  = "Cbr";
//static const char       *ker_pUserSignature  = "User";

static UINT8 m_aucMainMessagePool[sizeof(MESSAGE) * 16] = {0};
static HOST_MSG_HANDLE m_sHostMsgHandle    = {0};
static USER_TASK_INFO m_sUserCallbackHandle      = {0};

static USER_TASK_INFO *s_pUserCallbackHandleList = NULL;

void SendMessage(HMESSAGEQUEUE hMessageQueue, MSGID_T MessageClass, MSGID_T MessageID, MSGPARAM_T Parameter);
void ReceiveMessage(HMESSAGEQUEUE hMessageQueue, MSGID_T *pMessageClass, MSGID_T *pMessageID, MSGPARAM_T *pParameter);

HUSERCALLBACK UserCallbackDispatcher_CreateHandle(PUSER_TASK_INFO handle,HTASK hTask, HMESSAGEQUEUE hMessageQueue);

VOID UserCallbackDispatcher_Dispatch(MSGID_T MessageID, MSGPARAM_T Parameter)
{
	P_USER_CALLBACK pcbr = (P_USER_CALLBACK)MessageID;
	VOID *pparam = Parameter;

	ASSERT(pcbr != NULL);

	(*pcbr)(pparam);
}

VOID *
MainTask( UINT32 uwParam1,
              UINT32 uwParam2,
              UINT32 uwParam3,
              UINT32 uwParam4 )
{
    MESSAGE msg;
    
    while (1)
    {
        ReceiveMessage( (HMESSAGEQUEUE)m_mainQ, 
                        &msg.MessageClass, 
                        &msg.MessageID,
                        &msg.Parameter );

        TRACE("msgClass = %d msgid = 0x%08x param = %d\n", msg.MessageClass,msg.MessageID, msg.Parameter);

        switch( msg.MessageClass )
        {
            case MESSAGE_HANDLER_ID_HOST:
                HostMessageHandler_Distributor(msg.MessageID, 
                                               msg.Parameter);
                break;
            case MESSAGE_HANDLER_ID_USER:
            
                UserCallbackDispatcher_Dispatch(msg.MessageID, 
                                                msg.Parameter);
                break;
            default:
                break;
        }
        
        LOS_TaskDelay(5);
    }
}

void MainTask_Init(void)
{
    UINT32 uwRet = LOS_OK;
    
    TSK_INIT_PARAM_S stTaskInitParam;
    
    (VOID)memset((void *)(&stTaskInitParam), 0, sizeof(TSK_INIT_PARAM_S));
    stTaskInitParam.pfnTaskEntry    = MainTask;
    stTaskInitParam.uwStackSize     = LOSCFG_BASE_CORE_TSK_IDLE_STACK_SIZE;
    stTaskInitParam.pcName          = "mainTask";
    stTaskInitParam.usTaskPrio      = LOS_TASK_PRIORITY_HIGHEST + 5;
    
    uwRet += LOS_TaskCreate(&m_mainTaskId, &stTaskInitParam);    
    uwRet += LOS_QueueCreate("queue", 6, &m_mainQ, 0, 4*4);
    uwRet += LOS_MemboxInit(m_aucMainMessagePool, sizeof(m_aucMainMessagePool), sizeof(MESSAGE) );
    ASSERT( LOS_OK == uwRet );

    HostMessageHandler_CreateHandle(&m_sHostMsgHandle, (HTASK)m_mainTaskId, (HMESSAGEQUEUE)m_mainQ );
    UserCallbackDispatcher_CreateHandle(&m_sUserCallbackHandle, (HTASK)m_mainTaskId, (HMESSAGEQUEUE)m_mainQ );

    TRACE("MainTask_Init\n");
}

void mainTaskCreate(void)
{
}

HOST_MSG_HANDLE *
MainTask_GetHandle(void)
{
    return &m_sHostMsgHandle;
}

void MainTask_SendMessage( uint32_t msgID, uint32_t msgParam )
{
    if ( m_mainTaskId )
    {
        SendMessage((HMESSAGEQUEUE)m_mainQ, MESSAGE_HANDLER_ID_HOST, (MSGID_T)msgID, (MSGPARAM_T)msgParam);
    }
}

void ReceiveMessage(HMESSAGEQUEUE hMessageQueue, MSGID_T *pMessageClass, MSGID_T *pMessageID, MSGPARAM_T *pParameter)
{
    uint16_t uwRet;
    MESSAGE *pMsg;

    ASSERT( NULL != pMessageClass );
    ASSERT( NULL != pMessageID );
    ASSERT( NULL != pParameter );
    
    uwRet = LOS_QueueRead((UINT32)hMessageQueue, &pMsg, sizeof(UINT32), LOS_WAIT_FOREVER);
    ASSERT( LOS_OK == uwRet);

    if ( NULL != pMsg )
    {
        ASSERT( pMsg->pSignature == ker_pMsgSignature );
        
        *pMessageClass = pMsg->MessageClass;
        *pMessageID    = pMsg->MessageID;
        *pParameter    = pMsg->Parameter;
    
        uwRet = LOS_MemboxFree(m_aucMainMessagePool, pMsg);

        ASSERT( LOS_OK == uwRet );
        
        return ;
    }

    ASSERT( FALSE );
}

void SendMessage(HMESSAGEQUEUE hMessageQueue, MSGID_T MessageClass, MSGID_T MessageID, MSGPARAM_T Parameter)
{
	MESSAGE	    *pmsg;
    UINT16          uwRet;
    
    pmsg = LOS_MemboxAlloc(m_aucMainMessagePool);
    ASSERT( NULL != pmsg);
    
	pmsg->pSignature    = ker_pMsgSignature;
	pmsg->MessageClass	= MessageClass;
	pmsg->MessageID		= MessageID;
	pmsg->Parameter		= Parameter;

    uwRet = LOS_QueueWrite((UINT32)hMessageQueue, pmsg, sizeof(UINT32), 0);
    if(uwRet != LOS_OK)
    {
        TRACE("SendMessage ERR 0x%x\n", uwRet);
    }   
    
	ASSERT(LOS_OK == uwRet);
}

HUSERCALLBACK UserCallbackDispatcher_CreateHandle(PUSER_TASK_INFO handle,HTASK hTask, HMESSAGEQUEUE hMessageQueue)
{
	PUSER_TASK_INFO pinfo = handle;

	//TakeSemaphore(s_hUserCallbackMutex);

	pinfo->pSignature = ker_pCbrSignature;
	pinfo->pNext = NULL;
	pinfo->hTask = hTask;
	pinfo->hMessageQueue = hMessageQueue;

	if(s_pUserCallbackHandleList == NULL)
	{
		s_pUserCallbackHandleList = pinfo;
	}
	else
	{
		PUSER_TASK_INFO ptmp = s_pUserCallbackHandleList;

		while(ptmp->pNext != NULL)
		{
			ptmp = ptmp->pNext;
		}
		ptmp->pNext = pinfo;
	}

	//GiveSemaphore(s_hUserCallbackMutex);

	return ((HUSERCALLBACK)pinfo);
}


VOID UserCallbackDispatcher_ExecuteCallback(P_USER_CALLBACK pUserCBR, VOID *pParameter)
{
	PUSER_TASK_INFO pinfo = (PUSER_TASK_INFO)&m_sUserCallbackHandle;

	ASSERT(pinfo->pSignature == ker_pCbrSignature);
	ASSERT(pUserCBR != NULL);

	SendMessage(pinfo->hMessageQueue, MESSAGE_HANDLER_ID_USER, (MSGID_T)pUserCBR, (MSGPARAM_T)pParameter);
}

