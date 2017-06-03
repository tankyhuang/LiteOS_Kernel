#ifndef _MAIN_TASK_H_
#define _MAIN_TASK_H_

#include "HostMessageHandler.h"

typedef struct USER_TASK_INFO
{
	const char *pSignature;
	struct USER_TASK_INFO *pNext;
	HTASK hTask;
	HMESSAGEQUEUE hMessageQueue;
} USER_TASK_INFO, *PUSER_TASK_INFO;


typedef void *HUSERCALLBACK;
typedef void (*P_USER_CALLBACK)(void *pParameter);

void MainTask_Init(void);
void mainTaskCreate(void);

HOST_MSG_HANDLE *
MainTask_GetHandle(void);

void MainTask_SendMessage( uint32_t msgID, uint32_t msgParam );

void UserCallbackDispatcher_ExecuteCallback(P_USER_CALLBACK pUserCBR, VOID *pParameter);

#endif

