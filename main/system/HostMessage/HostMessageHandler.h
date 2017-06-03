#ifndef _HOST_MESSAGE_HANDLER_H_
#define _HOST_MESSAGE_HANDLER_H_

#define MESSAGE_HANDLER_ID_HOST		0x0001
#define MESSAGE_HANDLER_ID_USER		0x0002


typedef int32_t	MSGID_T;
typedef void	*MSGPARAM_T;
typedef void	*HMESSAGEQUEUE;

typedef void	*HTASK;

typedef struct MESSAGE
{
	const char		*pSignature;
	MSGID_T			MessageClass;
	MSGID_T			MessageID;
	MSGPARAM_T		Parameter;
} MESSAGE, *PMESSAGE;

typedef struct HOST_MSG_HANDLE
{
	const CHAR *pSignature;
	struct HOST_MSG_HANDLE *pNext;
	HTASK hTask;
	HMESSAGEQUEUE hMessageQueue;
} HOST_MSG_HANDLE, *PHOST_MSG_HANDLE;

typedef void *HHOSTMSG;
typedef void (*P_HOST_MSG_CBR)(uint32_t Param, void *pUserInstance);

HHOSTMSG HostMessageHandler_CreateHandle(PHOST_MSG_HANDLE handle, HTASK hTask, HMESSAGEQUEUE hMessageQueue);


#endif //_HOST_MESSAGE_HANDLER_H_

