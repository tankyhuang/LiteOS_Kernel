#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <debug.h>
#include <bsp_usart.h>
#include "los_task.h"
#include "los_mux.h"

#if DEBUG_ENABLE 

#define MAX_PRINT 129

static UINT32 m_MutextPrint = 0;

void debugInit( void )
{
    UINT32 uwRet;

    uwRet = LOS_MuxCreate(&m_MutextPrint);
    if ( LOS_OK != uwRet )
    {
        printf("LOS_MuxCreate ERR = %d\n", uwRet);
        return ;
    }
}

VOID
blockPrintf( CHAR* szText, UINT16 uwLength )
{
    //OsMutexGet( &txBlockingMutex, TX_WAIT_FOREVER );
    UINT32 uwRet;

    uwRet = LOS_MuxPend( m_MutextPrint, LOS_WAIT_FOREVER );
    if ( LOS_OK != uwRet )
    {
        printf("LOS_MuxPend ERR = %d\n", uwRet);
        return ;
    }

    // printf to the UART Monitor
    if (szText[uwLength - 1] == '\n')
    {
        // incase we have a line feed at the end, make sure we write at least one
        // carriage return
        UartWriteFifo( szText, uwLength - 1, 0);
        UartWriteFifo( "\r\n", 2, 0);
    }
    else
    {
        UartWriteFifo(szText, uwLength, 0 );
    }

    LOS_MuxPost( m_MutextPrint );
}

VOID
dbgPrint( const CHAR* szFmt, ...)
{
    va_list ap;

    UINT32 uiLen;

    CHAR szText[MAX_PRINT];

    va_start(ap, szFmt);

    // Build debug string.
    vsnprintf( szText, MAX_PRINT-1, szFmt, ap );

    va_end(ap);

    szText[ MAX_PRINT - 2 ] = '\n';
    szText[ MAX_PRINT - 1 ] = '\0';

    // Send the debug string to the port
    uiLen = strlen(szText);

    blockPrintf(szText, uiLen);
}

#endif

