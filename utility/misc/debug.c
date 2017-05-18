#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <debug.h>
#include <freq.h>
#include <bsp_usart.h>
#include "los_task.h"
#include "los_mux.h"

#if DEBUG_ENABLE 

#define MAX_PRINT 128

static uint32_t m_MutextPrint = 0;
static uint8_t  m_debugEnable = true;

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
blockPrintf( char* str, UINT16 len )
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
    if (str[len - 1] == '\n')
    {
        // incase we have a line feed at the end, make sure we write at least one
        // carriage return
        UartWriteFifo( str, len - 1, 0);
        UartWriteFifo( "\r\n", 2, 0);
    }
    else
    {
        UartWriteFifo(str, len, 0 );
    }

    LOS_MuxPost( m_MutextPrint );
}

VOID
dbg_print( const char* format, ...)
{
    va_list ap;

    UINT32 uiLen;

    CHAR szText[MAX_PRINT];

    va_start(ap, format);

    // Build debug string.
    vsnprintf( szText, MAX_PRINT-1, format, ap );

    va_end(ap);

    szText[ MAX_PRINT - 2 ] = '\n';
    szText[ MAX_PRINT - 1 ] = '\0';

    // Send the debug string to the port
    uiLen = strlen(szText);

    blockPrintf(szText, uiLen);
}


void _log_d(const CHAR* format, ...)
{
    va_list ap;

    uint32_t len;

    char szText[MAX_PRINT] = {0};

    len = sprintf(szText, "%08d [%-8.8s]", (uint32_t)LOS_Tick2MS(LOS_TickCountGet())/*/GET_SYS_FREQ_KHZ()*/, (char *)LOS_CurTaskNameGet() );
    va_start(ap, format);

    // Build debug string.
    len += vsnprintf( szText + len, MAX_PRINT-len-1, format, ap );

    va_end(ap);

    szText[ MAX_PRINT - 2 ] = '\n';
    szText[ MAX_PRINT - 1 ] = '\0';

    if ( m_debugEnable )
    {
        dbg_print("%s", szText);
    }
}

void assert_failed(uint8_t* file, uint32_t line)
{
    dbg_print("assert failed: file %s  line %d\n");
}


#endif

