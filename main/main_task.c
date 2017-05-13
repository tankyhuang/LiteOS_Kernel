#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <debug.h>
#include <console.h>
#include "los_task.h"
#include "los_queue.h"

static UINT32 m_mainTaskId = 0;
static UINT32 m_mainQ      = 0;

CHAR message[] = "test is message x";

VOID *
mainTaskLoop( UINT32 uwParam1,
              UINT32 uwParam2,
              UINT32 uwParam3,
              UINT32 uwParam4 )
{
    UINT32 msgID;
    UINT32 uwRet = 0;
    UINT32 uwMsgCount = 0;

    while (1)
    {
        uwRet = LOS_QueueRead(m_mainQ, &msgID, 4, LOS_WAIT_FOREVER);
        if(uwRet != LOS_OK)
        {
            LOG("recv message failure,error:%x\n",uwRet);
            break;
        }
		else
		{
			uwMsgCount++;
		}
        LOG("msgid = %d\n", msgID);

        switch( msgID )
        {
            case 1:
                //LOG("msg 1\n");
                break;
            case 2:
                //LOG("msg 2\n");
                break;
            default:
                break;
        }
        
        LOS_TaskDelay(5);
    }

    return 0;
}

void mainTaskInitialize(void)
{
    UINT32 uwRet;
    
    TSK_INIT_PARAM_S stTaskInitParam;
    
    (VOID)memset((void *)(&stTaskInitParam), 0, sizeof(TSK_INIT_PARAM_S));
    stTaskInitParam.pfnTaskEntry    = mainTaskLoop;
    stTaskInitParam.uwStackSize     = LOSCFG_BASE_CORE_TSK_IDLE_STACK_SIZE;
    stTaskInitParam.pcName          = "mainTask";
    stTaskInitParam.usTaskPrio      = LOS_TASK_PRIORITY_HIGHEST + 5;
    
    uwRet = LOS_TaskCreate(&m_mainTaskId, &stTaskInitParam);
    if (uwRet != LOS_OK)
    {
        LOG("main task create failed\n");
    }

    uwRet = LOS_QueueCreate("queue", 5, &m_mainQ, 0, 50);
    if(uwRet != LOS_OK)
    {
        LOG("mainQ create failed %x\n", uwRet);
    }

    LOG("mainTaskInitialize\n");
}

void mainTaskCreate(void)
{
}

void mainTaskSendMsg( void* msg, UINT32 param )
{
    UINT32 uwRet = 0;
   
    if ( m_mainTaskId )
    {
        uwRet = LOS_QueueWrite(m_mainQ, msg, sizeof(message), 0);
        if(uwRet != LOS_OK)
        {
            LOG("mainTaskSendMsg ERR 0x%x\n", uwRet);
        }    
    }
}

