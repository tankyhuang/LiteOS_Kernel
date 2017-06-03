#include "stm32f10x.h"
#include "stdlib.h"
#include "string.h"
#include "types.h"
#include "debug.h"

#include "los_event.h"
#include "los_task.h"
#include "los_queue.h"
#include "los_swtmr.h"
#include "gpio_conf.h"
#include "hook.h"
#include "SystemMonitor.h"


// MACRO
#define MONITOR_EVENT_wait1     0x00000001
#define MONITOR_EVENT_wait2     0x00000002
#define MONITOR_EVENT_TaskMsg   0x00000004


#define MONITOR_EVENT_ALL      MONITOR_EVENT_wait1 | \
                               MONITOR_EVENT_wait2 | \
                               MONITOR_EVENT_TaskMsg
// TYPE DEFINITIONS
static UINT32 m_monitorTaskId = 0;
static UINT32 m_monitorQ      = 0;
static HHOOK  s_hRedRayStatusNotifyCBRHook       = NULL;
static EVENT_CB_S  m_monitorEvent = {0};

static SYS_RED_RAY_STATUS  s_CurrentRedRayStatus  = sysRedRayStatus_Unknown;
static SYS_RED_RAY_STATUS  s_PreviousRedRayStatus = sysRedRayStatus_Unknown;

void MonitorTask_SendMessage( void* msg, UINT32 param );
void NotifyRedRayStatus(SYS_RED_RAY_STATUS status);
static void onRedRayRoutine( void );

void ReyCheck_Callback(UINT32 arg)
{
    LOS_EventWrite( &m_monitorEvent, MONITOR_EVENT_wait1 );
}

static void StartRayCheck( void )
{
    uint16_t ret;
    uint16_t timerID;
    ret = LOS_SwtmrCreate(200, LOS_SWTMR_MODE_PERIOD,ReyCheck_Callback,&timerID,1);
    ASSERT(LOS_OK == ret);

    LOS_SwtmrStart( timerID );
}


// FUNCTION DECLARATIONS
static void onTaskMessage( void )
{
    UINT32 msgID;
    UINT32 uwRet = 0;
    UINT32 uwMsgCount = 0;

    uwRet = LOS_QueueRead(m_monitorQ, &msgID, 4, LOS_WAIT_FOREVER);
    if(uwRet != LOS_OK)
    {
        TRACE("recv message failure,error:%x\n",uwRet);      
    }
    else
    {
        uwMsgCount++;
    }
    TRACE("msgid = %d\n", msgID);
    
    switch( msgID )
    {
        case 1:
            TRACE("msg 1\n");
            StartRayCheck();
            break;
        case 2:
            //TRACE("msg 2\n");
            break;
        default:
            break;
    }

}

void *
MonitorTask(  UINT32 uwParam1,
              UINT32 uwParam2,
              UINT32 uwParam3,
              UINT32 uwParam4 )
{
    UINT32 uwEvent;

    while (1)
    {
         uwEvent = LOS_EventRead( &m_monitorEvent, 
                                  MONITOR_EVENT_ALL, 
                                  LOS_WAITMODE_OR, 
                                  LOS_WAIT_FOREVER/*|LOS_WAITMODE_CLR*/);

         //TRACE("Monitor event 0x%x \n", uwEvent);                                  
  
         LOS_EventClear( &m_monitorEvent, ~(uwEvent & MONITOR_EVENT_ALL) );
         if(uwEvent & MONITOR_EVENT_wait1)
         {
            onRedRayRoutine();
         }
         else if ( uwEvent & MONITOR_EVENT_wait2 )
         {
             TRACE("Monitor event 2 \n");
         }
         else if ( uwEvent & MONITOR_EVENT_TaskMsg )
         {
             TRACE("Monitor task msg \n");
             onTaskMessage();
         }
         else
         {
         }
         
         uwEvent &= ~uwEvent;
    }
}

void RedRayInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(GPIO_VOID_R_CLK|GPIO_VOID_L_CLK, ENABLE);
			
	GPIO_InitStructure.GPIO_Pin     = GPIO_VOID_R_PIN;       
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IPU;   
	GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_VOID_R_GPIO , &GPIO_InitStructure); 
	
	GPIO_InitStructure.GPIO_Pin     = GPIO_VOID_L_PIN;
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_VOID_L_GPIO , &GPIO_InitStructure); 
}

static void onRedRayRoutine( void )
{
    uint8_t left;
    uint8_t right;
    
    left = GPIO_ReadInputDataBit(GPIO_VOID_L_GPIO, GPIO_VOID_L_PIN);
    if ( BARRIER_Y == left )
    {
        //TRACE("Red Ray L  - %d \n", left);
        left = sysRedRayStatus_L_True;
    }
    else
    {
        left = sysRedRayStatus_None;        
    }
    
    right = GPIO_ReadInputDataBit(GPIO_VOID_R_GPIO, GPIO_VOID_R_PIN);
    if ( BARRIER_Y == right )
    {
        //TRACE("Red Ray R  - %d \n", right);
        right = sysRedRayStatus_R_True;
    }
    else
    {
        right = sysRedRayStatus_None;
    }
    
    s_CurrentRedRayStatus = (SYS_RED_RAY_STATUS)(left + right);

    if( s_CurrentRedRayStatus != s_PreviousRedRayStatus )
    {
        s_PreviousRedRayStatus = s_CurrentRedRayStatus;
        NotifyRedRayStatus( s_CurrentRedRayStatus );
    }
}

void 
SYSTEM_MONITOR_Init( void )
{
    uint32_t uwRet;
    
    TSK_INIT_PARAM_S stTaskInitParam;
    
    memset((void *)(&stTaskInitParam), 0, sizeof(TSK_INIT_PARAM_S));
    stTaskInitParam.pfnTaskEntry    = MonitorTask;
    stTaskInitParam.uwStackSize     = LOSCFG_BASE_CORE_TSK_IDLE_STACK_SIZE;
    stTaskInitParam.pcName          = "MoniTask";
    stTaskInitParam.usTaskPrio      = LOS_TASK_PRIORITY_LOWEST - 3;
    
    uwRet = LOS_TaskCreate(&m_monitorTaskId, &stTaskInitParam);
    ASSERT( LOS_OK == uwRet );

    uwRet = LOS_EventInit(&m_monitorEvent);
    ASSERT( LOS_OK == uwRet );

    uwRet = LOS_QueueCreate("queue", 6, &m_monitorQ, 0, 4*4);
    ASSERT( LOS_OK == uwRet );

    RedRayInit();
    
    //TRACE("MoniEvent b 0x%x\n", m_monitorEvent.uwEventID);
	 
    //LOS_EventWrite( &m_monitorEvent, MONITOR_EVENT_wait1 );
    //LOS_EventWrite( &m_monitorEvent, MONITOR_EVENT_wait2 );
    //TRACE("MoniEvent a 0x%x\n", m_monitorEvent.uwEventID);

    s_hRedRayStatusNotifyCBRHook = CreateHook();
    
    MonitorTask_SendMessage((void*)0x1, 0 );
		
	TRACE("MoniTask init\n");
    
    
}

HSYSM_RED_RAY_STATUS_CBR SM_RegisterRedRayStatusCBR( PSYSM_RED_RAY_STATUS_CBR pRedRayStatusCBR, void* pUserInstance )
{
    return (HSYSM_RED_RAY_STATUS_CBR)AddHookProcedure(s_hRedRayStatusNotifyCBRHook, (PHOOKPROC)pRedRayStatusCBR, pUserInstance);
}

void NotifyRedRayStatus(SYS_RED_RAY_STATUS status)
{
    ASSERT( status < SYS_RED_RAY_STATUS_COUNT );

    TRACE("NotifyRedRayStatus %d\n", status);

    ExecuteHookProcedure(s_hRedRayStatusNotifyCBRHook, status);
}

// VARIABLE DEFINITIONS

// FUNCTION DEFINITIONS
void MonitorTask_SendMessage( void* msg, UINT32 param )
{
    UINT32 uwRet = 0;
   
    if ( m_monitorTaskId )
    {
        uwRet = LOS_QueueWrite(m_monitorQ, msg, 4, 0);
        if(uwRet != LOS_OK)
        {
            TRACE("MonitorTask_SendMessage ERR 0x%x\n", uwRet);
        }
        LOS_EventWrite( &m_monitorEvent, MONITOR_EVENT_TaskMsg);    
    }
}


// COMMON THREDS.


// LOCAL THREADS.


