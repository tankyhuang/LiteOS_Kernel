#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <debug.h>
#include <console.h>
#include <bsp_usart.h>
#include "los_task.h"

#if DEBUG_ENABLE

#define MAX_CONSOLE_FUNCTION_LISTS      10
#define	MON_MAX_SCRIPT_NUM              10
#define PROMPT_LABEL_STR_MAX_LEN        64

#define CHAR_NULL_TERM		    		'\0'            // Null-Term char
#define CHAR_BACK_SPACE		    		'\b'
#define CHAR_TAB			    		'\t'
#define CHAR_NEW_LINE		    		'\n'
#define CHAR_CRET						'\r'
#define CHAR_WSPACE			    		' '


typedef struct 
{
	char*   str;
	int     len;
	int     pos;
	UINT32  maxLen;
} SCMDCONTEXT;

typedef struct 
{
	char printBuf[CONSOLE_LINE_MAX_LENGTH * 2];
	bool isSilentMode;
	bool isIoSimMode;
} SSysOutputCtx;


// This array holds all the commands which was registered.
static PSCONSOLECOMMAND sConsoleFunctionList[MAX_CONSOLE_FUNCTION_LISTS];

// Holds the next free entry for function list. It also counts the number of
// lists that are currently registered (starting from 0).
static UINT32 m_uiConsoleListCount = 0;

// Prompt
static char     szPrompt[CONSOLE_PROMPT_MAX_LENGTH]     = {0};
static char     szConsoleLine[CONSOLE_LINE_MAX_LENGTH]  = {0};
static char     szLastCommand[CONSOLE_LINE_MAX_LENGTH]  = {0};


static VOID consoleHelp(CHAR* szParam);
static VOID consoleList(CHAR* szParam);
static VOID consoleMain(VOID);

static SCMDCONTEXT* psConsoleContext = NULL;
static SCMDCONTEXT  sConsoleContext  = {0};
static BOOL         m_bInsertMode    = FALSE;

static SSysOutputCtx g_outputCtx = { 0 };


static EKeyType eTerminateKeys[] = { 
    KT_TAB,	
    KT_UP, 
    KT_DOWN, 
    KT_QUESTION, 
    KT_SLASH, 
    KT_NEW_LINE, 
    KT_NUM_KEYS
};

static SCONSOLECOMMAND sConsoleDefaultList[] =
{
   {  "help",   "<name>",               "Prints function help",                   consoleHelp },
   {  "ls",     "[prefix]",             "Prints all avaliable functions help",    consoleList },

   {  0, 0, 0, 0 }
};


void consoleSyncPrintf( char* buf, int count )
{
    if ( count == -1 )
    {
        count = strlen( buf );
    }
    
    blockPrintf( buf, count );
}

BOOL consoleMoveCaretLeft( )
{
    char ch = CHAR_BACK_SPACE;
	if ( psConsoleContext->pos > 0 )
	{
        consoleSyncPrintf( &ch, 1 );
		psConsoleContext->pos--;
		return true;
	}
	return false;
}

BOOL consoleMoveCaretRight( )
{
	if ( psConsoleContext->pos < psConsoleContext->len )
	{
        consoleSyncPrintf( psConsoleContext->str + psConsoleContext->pos, 1 );
		psConsoleContext->pos++;
		return true;
	}
	return false;
}

void consoleMoveCaretHome( )
{
	while ( consoleMoveCaretLeft() ) ; 
}

void consoleMoveCaretEnd( )
{
    while ( consoleMoveCaretRight() ) ; 
}

void consoleResetCommand()
{
    psConsoleContext->pos = 0;
    psConsoleContext->len = 0;
    *psConsoleContext->str = CHAR_NULL_TERM;
}

VOID
consoleActivate()
{   
    UINT32 uwTaskID = 0;
    UINT32 uwRet;

    // Set default prompt
    strcpy( szPrompt, "LiteOS" );

    // Show prompt to user. The user can enter a new command.
    printf( "%s> ", szPrompt);

    TSK_INIT_PARAM_S stTaskInitParam;

    (VOID)memset((void *)(&stTaskInitParam), 0, sizeof(TSK_INIT_PARAM_S));
    stTaskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)consoleMain;
    stTaskInitParam.uwStackSize  = LOSCFG_BASE_CORE_TSK_IDLE_STACK_SIZE;
    stTaskInitParam.pcName       = "Console";
    stTaskInitParam.usTaskPrio   = 10;
    stTaskInitParam.uwResved     = LOS_TASK_STATUS_DETACHED;
    
    uwRet = LOS_TaskCreate(&uwTaskID, &stTaskInitParam);

    if (uwRet != LOS_OK)
    {
        printf("Console task create failed\n");
    }
}

BOOL
consoleInit( void )
{
    m_uiConsoleListCount = 0;
    //printf_red("ConsoleInit\n");
	
    // Register default commands
    consoleRegisterFunctions( sConsoleDefaultList );

    m_bInsertMode = TRUE;

    memset( &sConsoleContext, 0, sizeof(SCMDCONTEXT) );
    sConsoleContext.str     = szConsoleLine;
    sConsoleContext.maxLen  = CONSOLE_LINE_MAX_LENGTH;
    sConsoleContext.len     = strlen( sConsoleContext.str );
    sConsoleContext.pos     = 0;

    psConsoleContext        = &sConsoleContext;  
    
    return TRUE;
}

BOOL
consoleRegisterFunctions(PSCONSOLECOMMAND psList)
{
    // Check if there are still free enteries.
    if ( m_uiConsoleListCount == MAX_CONSOLE_FUNCTION_LISTS )
    {
        // No more commands lists.
        printf("No more commands lists\r\n");

        return FALSE;
    }


    // Add new commands list. From this point all commands from this list
    // are avaliable to the user.
    sConsoleFunctionList[ m_uiConsoleListCount++ ] = psList;

    return TRUE;
}

BOOL
consoleParseAndExec(CHAR* szCommand)
{
    UINT32 i;

    BOOL bFound = FALSE;

    // Comand name string
    CHAR szName[CONSOLE_LINE_MAX_LENGTH];
	CHAR szCommandName[CONSOLE_LINE_MAX_LENGTH];

    // Gets the command name.
    sscanf( szCommand, "%s", szName );

    // Search for the command in all lists.
    for ( i = 0; i < m_uiConsoleListCount && bFound == FALSE; i++ )
    {
        PSCONSOLECOMMAND psCmd = sConsoleFunctionList[i];

        for ( psCmd = sConsoleFunctionList[i];
              psCmd->szName != NULL;
              psCmd++ )
        {
            sscanf( psCmd->szName, "%s", szCommandName );

            // Search for command in list.
            if ( strcmp( szName, szCommandName ) == 0 )
            {
                CHAR* szCommandEnd;
                CHAR* szCommandEndC10;
                CHAR  szCommandSave[4];
                BOOL  bAsyncExec = FALSE;
  
                szCommand += strlen(szName);

                // Skip leading white-spaces
                while( *szCommand == ' ' || *szCommand == '\t' )
                {
                    szCommand++;
                }

                szCommandEnd = szCommand + strlen(szCommand) - 1;

                // Remove trailing white-spaces
                while( *szCommandEnd == ' ' || *szCommandEnd == '\t' )
                {
                    // Did we reach the beginning of the string?
                    if ( szCommandEnd <= szCommand )
                    {
                        break;
                    }

                    *szCommandEnd = '\0';
                    szCommandEnd--;
                }

                // :PATCH: Add terminator to the string, so that stdlib
                // will not cause TLB exception on _eat_white
                szCommandEndC10 = szCommandEnd;
                // Save before overwriting
                memcpy( szCommandSave, szCommandEndC10+1, 4 );
                szCommandEnd++;
                *szCommandEnd = ' ';
                szCommandEnd++;
                *szCommandEnd = '\0';
                szCommandEnd++;
                *szCommandEnd = '.';
                szCommandEnd++;
                *szCommandEnd = '\0';

                // Check if async execution should be done?
                if ( strlen(szCommand) > 0 )
                {
                    if ( *szCommandEndC10 == '&' )
                    {
                        bAsyncExec = TRUE;
                    }
                }

                // Check if async execution should be done and priority is given?
                if ( strlen(szCommand) > 1 )
                {
                    UINT32 uiTemp;

                    uiTemp = *szCommandEndC10 - '0';

                    if ( /*uiTemp >= 0 && */uiTemp <= 9 )
                    {
                        if ( *(szCommandEndC10-1) == '&' )
                        {
                            bAsyncExec = TRUE;
                        }
                    }
                }

                if ( bAsyncExec == TRUE )
                {
                    // Will cause TLB exception on eat_white
                    //*szCommandEndC10 = '\0';

                    // Create thread to run the command.
                }
                else
                {
                    // Run command.
                    psCmd->fnFunction( szCommand );
                }

                // Restore overwritten part
                memcpy( szCommandEndC10+1, szCommandSave, 4 );

                // Command was found and run.
                bFound = TRUE;

                break;
            }
        }

    }
    
    return bFound;
}

VOID
consoleParse(CHAR* szCommand)
{
    UINT32 uiCurrChar = 0;

    BOOL bFound = FALSE;

    printf( "\n" );

    // Check if there is a new command
    if ( szCommand[0] != 0 )
    {
        while ( 1 )
        {
            // If it's not the first time we try
            if ( uiCurrChar > 0 )
            {
                // Swap the current char the with previous char
                CHAR chTmp = szCommand[uiCurrChar];
                szCommand[uiCurrChar] = szCommand[uiCurrChar-1];
                szCommand[uiCurrChar-1] = chTmp;
            }

            bFound = consoleParseAndExec( szCommand );

            if ( bFound == TRUE )
            {
                break;
            }

            if ( uiCurrChar > 0 )
            {
                // Swap back the current char the with previous char
                CHAR chTmp = szCommand[uiCurrChar];
                szCommand[uiCurrChar] = szCommand[uiCurrChar-1];
                szCommand[uiCurrChar-1] = chTmp;
            }

            uiCurrChar++;

            if ( szCommand[uiCurrChar] == ' '  ||
                 szCommand[uiCurrChar] == '\t' ||
                 szCommand[uiCurrChar] == '\n' ||
                 szCommand[uiCurrChar] == '\r' ||
                 szCommand[uiCurrChar] == '\0' )
            {
                break;
            }
        }

        if ( bFound == FALSE )
        {
            // Gets the command name.
            sscanf( szCommand, "%s", szCommand );

            // Command end
            printf( "%s is not recognized as an internal command\r\n",
                    szCommand );
        }
    }

    // Show prompt to user. The user can enter a new command.
    printf( "%s> ", szPrompt);

}

VOID
consolePreParse(CHAR* szCommand)
{
    UINT32 i;
    UINT32 uiLength;

    uiLength = strlen( szCommand );

    // Parse and remove backspaces
    for ( i = 0; i < uiLength; i++ )
    {
        // Backspace?
        if ( szCommand[i] == 0x08 )
        {
            // Is it the first char?
            if ( i == 0 )
            {
                strcpy( szCommand, szCommand + 1 );
                i--;
                uiLength--;
            }
            else // i > 0
            {
                strcpy( szCommand + i - 1, szCommand + i + 1 );
                i -= 2;
                uiLength -= 2;
            }

            continue;
        }

        // Any other illegal char?
        if ( ( szCommand[i] == '`' ) &&
             ( szCommand[i+1] != 0x08 ) )
        {
            // Just ignore the char
            strcpy( szCommand + i, szCommand + i + 1 );
            i--;
            uiLength--;

            continue;
        }
    }

    // Parse and remove non alpha-numeric chars
    for ( i = 0; i < uiLength; i++ )
    {
        // Parse only text characters.
        if ( (szCommand[i] < ' ') || (szCommand[i] > '~') )
        {
            // Replace "end of line" with "end of string"
            if ( szCommand[i] == '\n' || szCommand[i] == '\r' )
            {
                szCommand[i] = '\0';
            }

            // Change non alpha-numeric chars to space.
            if ( szCommand[i] != '\0' )
            {
                szCommand[i] = ' ';
            }
        }
    }

    if ( strcmp( szCommand, "rep" ) == 0 ||
         strcmp( szCommand, "+" )   == 0 ||
         strcmp( szCommand, "=" )   == 0 ||
         strcmp( szCommand, "," )   == 0 )
    {
        strcpy( szCommand, szLastCommand );
    }
    else if ( strcmp( szCommand, "" ) != 0 )
    {
        strcpy( szLastCommand, szCommand );
    }

}

VOID
consolePrompt(CHAR* szParam)
{
    if( NULL != szParam )
    {
        strncpy( szPrompt, szParam, CONSOLE_LINE_MAX_LENGTH );
    }
}

VOID
consoleDump(CHAR* szParam)
{
}

BOOL
consoleRelease()
{
    return TRUE;
}

BOOL consoleReadInputCharsIBMKey( char* chReg, char* chExtend )
{ 
    CHAR buf[3] = {0};
    UINT32 read = 0;
    char ch;

    if ( UartReadFifo( buf, 1, (UINT32*)&read, 0 ) )
    {
        // No key pressed
        ch = *((char*)buf);
        *chReg = ch;
        return TRUE;
    }
    
    return false;
}


BOOL IsEditTermKey( EKeyType keyPress, EKeyType* termKeys )
{

	for ( ; *termKeys != KT_NUM_KEYS; *termKeys++ )
	{
		if ( *termKeys == keyPress )
		{
			return true;
		}
	}

	return false;
}

char* StrInsertCharTimes( char* str, int pos, UINT32 cnt, char insCh, int strLen )
{
	// e.g. on insert 2 spaces
	// abcdefg
	// 0123456
	// ab  cdefg
	// 012345678

	if ( strLen == - 1 )
	{
		strLen = strlen( str );
	}

	for ( int i = 0; i < strLen - pos; i++ )
	{
		str[strLen + cnt - i - 1] = str[strLen - i - 1];
	}

	str[strLen + cnt] = CHAR_NULL_TERM;
	while ( cnt-- )
	{
		str[pos + cnt] = insCh;
	}

	return str;
}

char* StrRemoveChars( char* str, int pos, UINT32 cnt, int strLen )
{
	// abcdefg
	// 0123456
	// abefg
	// 01234

	if ( strLen == - 1 )
	{
		strLen = strlen( str );
	}

	if ( pos + cnt < (UINT32)strLen )
	{
		strcpy( str + pos, str + pos + cnt );
	}
	else
	{
		str[strLen - cnt] = CHAR_NULL_TERM;
	}

	return str;
}

void PositionEditStr( int tempPos )
{
    int currPos = psConsoleContext->pos;
    psConsoleContext->pos = tempPos;

    while ( psConsoleContext->pos < currPos )
    {
        consoleMoveCaretRight();
    }
    while ( psConsoleContext->pos > currPos )
    {
        consoleMoveCaretLeft();
    }
}

void RemoveCharsFromStr( int cntChars )
{
	StrRemoveChars( psConsoleContext->str, psConsoleContext->pos, cntChars, psConsoleContext->len );
	psConsoleContext->len -= cntChars;
}

BOOL EraseCharBackSpace( )
{
    char szTempContext[CONSOLE_LINE_MAX_LENGTH]  = {0};

	if ( !consoleMoveCaretLeft() )
	{
		return false;
	}
	RemoveCharsFromStr( 1 );

    sprintf( szTempContext, "%s ", psConsoleContext->str + psConsoleContext->pos );
    consoleSyncPrintf( szTempContext, -1 );

	PositionEditStr( psConsoleContext->len + 1 );
	return true;
}

BOOL EraseCharDelete( )
{
    char szTempContext[CONSOLE_LINE_MAX_LENGTH]  = {0};

	if ( psConsoleContext->pos == psConsoleContext->len )
	{
		return false;
	}
	RemoveCharsFromStr( 1 );

	sprintf( szTempContext, "%s ", psConsoleContext->str + psConsoleContext->pos );
    consoleSyncPrintf( szTempContext, -1 );

	PositionEditStr( psConsoleContext->len + 1 );
	return true;
}

void InsertSpacesToStr( int cnt )
{
	StrInsertCharTimes( psConsoleContext->str, psConsoleContext->pos, cnt, CHAR_WSPACE, psConsoleContext->len );
}

bool InsertChar( char ch )
{
    if ( m_bInsertMode )
    {
        if ( psConsoleContext->len >= (int)psConsoleContext->maxLen )	// no room for new char
        {
            psConsoleContext->str[psConsoleContext->len-1] = CHAR_NULL_TERM;
            return false;
        }
        InsertSpacesToStr( 1 );
        psConsoleContext->str[psConsoleContext->pos] = ch;
        psConsoleContext->len++;
        consoleMoveCaretRight();
        if (psConsoleContext->pos < psConsoleContext->len)
        {
            consoleSyncPrintf( psConsoleContext->str + psConsoleContext->pos, -1 );
            PositionEditStr( psConsoleContext->len );
        }
    }
    else
    {
        if ( psConsoleContext->pos >= (int)psConsoleContext->maxLen )	// can't ins behind e-o-str
        {
            return false;
        }
        psConsoleContext->str[psConsoleContext->pos] = ch;
        if ( psConsoleContext->pos == psConsoleContext->len )
        {
            psConsoleContext->len++;
            psConsoleContext->str[psConsoleContext->len-1] = CHAR_NULL_TERM;
        }
        consoleMoveCaretRight();
    }

    return true;
}

EKeyType GetTranslatedKey( char* pressChar )
{
    char ch = 0;
    char chExt = 0;

    // Get command line input
    if ( consoleReadInputCharsIBMKey( &ch, &chExt ) )
    {
    	*pressChar = ch;
        switch ( ch )
        {
            case CHAR_TAB: return KT_TAB;
            case CHAR_BACK_SPACE: return KT_BACK;
    		case 10:
            case 13: return KT_NEW_LINE;
            case '?': return KT_QUESTION;
            case '/': return KT_SLASH;
        }

    	return KT_CHAR;    
    }
    
	// dangerous assumption (that everything else is a printable char) but satisfies our needs
    switch ( chExt )
    {
        case 80: return KT_DOWN;
        case 72: return KT_UP;
        case 75: return KT_LEFT;
        case 77: return KT_RIGHT;
        case 71: return KT_HOME;
        case 79: return KT_END;
        case 73: return KT_PGUP;
        case 81: return KT_PGDN;
        case 82: return KT_INSERT;
        case 83: return KT_DELETE;
    }
    
    return KT_NUM_KEYS; // untranslatable extended key
}

static EKeyType consoleCommandGet( void )
{
	EKeyType keyPress;
    BOOL isTerminateChar = FALSE;
	
	char ch;
    while( !isTerminateChar )
    {
        
        keyPress = GetTranslatedKey( &ch );
        if ( keyPress == KT_NUM_KEYS )
        {
            LOS_TaskDelay(50);
            continue;
        }
        
        isTerminateChar = IsEditTermKey( keyPress, eTerminateKeys );
        
        if ( !isTerminateChar )
        {
            switch ( keyPress )
            {
                case KT_LEFT:
                    break;
                case KT_HOME:
                    break;
                case KT_RIGHT:
                    break;
                case KT_END:
                    break;
                case KT_PGDN:
                    break;
                case KT_PGUP:
                    break;
                case KT_INSERT:
                    m_bInsertMode = !m_bInsertMode;
                    break;
                case KT_DELETE:
                    EraseCharDelete();
                    break;
                case KT_BACK:
                    EraseCharBackSpace();
                    break;
                case KT_CHAR:
                    InsertChar( ch );
                    break;
                default:
                    // a bit dangerous but i don't have the time to dwell into it. i simply assume
                    // here that the user won't input too "screen hazardous" scan codes...
                    InsertChar( ch );
            }
        }
    }

    return keyPress;
}

char SysGetChar( )
{
	char ch = 0;
	
	return ch;
}

void SysPutChar( char ch )
{
	putchar( ch );
}

int SysPrn( const char* format, ... )
{
	va_list arg;
	int totChars;

	va_start( arg, format );
	totChars = vsprintf( g_outputCtx.printBuf, format, arg );
	va_end(arg);

	printf( "%s", g_outputCtx.printBuf );

	return totChars;
}

VOID ParseCommandLine( CHAR *szCommand )
{
    consolePreParse( szCommand );
    consoleParse( szCommand );   
}

VOID
consoleMain(VOID)
{
	EKeyType retTermKey;
	BOOL bFirstRun = true;
    UINT32 num = 1;
    // We MUST set non-buffering IO for stdin or will get a crash in libc .
    //setvbuf(stdin, (CHAR*)rx_buf,_IOLBF, 1024);
    
    // Get commands from user and run them.
    while (TRUE)
    {
        // Wait for a command from the UART to be received.
        consoleResetCommand();
        
        retTermKey = consoleCommandGet();
        if ( bFirstRun && sConsoleContext.len == 0 )
        {
            // added as a request from users to push forward the basic usage of the new CLI
            SysPutChar( CHAR_NEW_LINE );
            SysPrn( "*** Listing available Meta-Commands: ***\n" );
            SysPutChar( CHAR_NEW_LINE );
            //ListCommands( -1, "!" );
            SysPutChar( CHAR_NEW_LINE );
            bFirstRun = FALSE;
        }

        switch ( retTermKey )
        {
        	case KT_TAB:
                //ParseTabComplete();
                break;
        	case KT_QUESTION:
        		 //ParseQuestionMark();
        	     break;
        	case KT_SLASH:
        		 //ParseSlashMark();
        		 break;
        	case KT_UP:
        		 //isLastWasHistScan = true;
        		 //TravelHistory( TH_PREV );
        		 break;
        	case KT_DOWN:
        		 //isLastWasHistScan = true;
        		 //TravelHistory( TH_NEXT );
        		 break;
        	case KT_NEW_LINE:
                ParseCommandLine( psConsoleContext->str );
                //mainTaskSendMsg( (void*)num, 4 );
                num++;
                
                break;
        	default:
        			 break;
        }
    }
}

VOID
consoleList(CHAR* szParam)
{
    UINT32 i;

    CHAR szSubstring[64] = {0};

    BOOL bSubstring = FALSE;

    if ( sscanf( szParam, "%s ", szSubstring ) == 1 )
    {
        bSubstring = TRUE;
    }

    // Search for the command in all of the lists
    for ( i = 0; i < m_uiConsoleListCount; i++ )
    {
        PSCONSOLECOMMAND psCmd;

        for ( psCmd = sConsoleFunctionList[i]; psCmd->szName != NULL; psCmd++ )
        {
            if ( bSubstring == FALSE ||
                 strstr( psCmd->szName, szSubstring ))
            {
                printf( "%s -- %s\r\n",
                        psCmd->szName,
                        psCmd->szHelp );
            }
        }
    }
}

VOID
consoleHelp(CHAR* szParam)
{
    UINT32 i;
    char* szCmd;

    // Check if we have any parameter (the command queried) and cleanup white spaces around it
    // First, skip spaces before
    for (szCmd = szParam ; *szCmd == ' ' ; ++szCmd );
    if (*szCmd == 0) // Reached end of string...
    { // If no command given, list all commands.
        consoleList(szParam);
        return;
    }
    // ... then clean spaces at end
    for (i = strlen(szCmd)-1; szCmd[i] == ' '; --i); // Move in reverse from string end until first non-space char
    szCmd[i+1] = 0; // Clean spaces at string end (NULL terminate)

    // Search for the command in all of the lists
    for ( i = 0; i < m_uiConsoleListCount; i++ )
    {
        PSCONSOLECOMMAND psCmd;

        for ( psCmd = sConsoleFunctionList[i]; psCmd->szName != NULL; psCmd++ )
        {
            if ( strcmp( psCmd->szName, szCmd ) == 0 )
            {
                printf( "\n" );
                printf( psCmd->szHelp );
                printf( "\n" );

                printf( "Usage: %s %s\n\n",
                        psCmd->szName,
                        psCmd->szArgumentList );

                return;
            }
        }
    }

    printf( "\"%s\" is not recognized as an internal command. Use \"ls\" to list commands.\n", szCmd );
}

#endif

