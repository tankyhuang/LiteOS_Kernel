#ifndef __CONSOLE_H
#define __CONSOLE_H

// Determine if a C++ compiler is being used.  If so, ensure that standard
// C is used to process the API information.
#ifdef __cplusplus
extern "C" {
#endif

// Port specifc file.
#include "los_typedef.h"

#define CONSOLE_LINE_MAX_LENGTH             64
#define CONSOLE_PROMPT_MAX_LENGTH           8

#define MON_DEFAULT_PROMPT_STR      		"$s"

typedef enum
{
	KT_CHAR = 0,
	KT_TAB,
	KT_BACK,
	KT_NEW_LINE,
	KT_UP,
	KT_DOWN,
	KT_LEFT,
	KT_RIGHT,
	KT_HOME,
	KT_END,
	KT_PGUP,
	KT_PGDN,
	KT_INSERT,
	KT_DELETE,
	KT_QUESTION,
    KT_SLASH,

	KT_NUM_KEYS
} EKeyType;


enum
{
    MAX_MON_CMD_LISTS = 64,
    MAX_MON_CMD_LIST_ENTRIES = 2048,

    DEF_HIST_ENT_LEN = 96,
    DEF_HIST_ENTRIES = 128,

    MAX_REPEAT_RUN_COUNT = 2000,
    PROMPT_FMT_STR_MAX_LEN = 32,
    NUM_TEMP_STACK_STR = 10,     // due to script possible recursion

    MAX_CMD_ARG_STR_LEN = 64, 
    MAX_FILE_NAME_LEN = 40,
    MAX_FILE_EXT_LEN = 10,

    MAX_SCRIPT_NAME_STR_LEN = 40,
    MAX_SCRIPT_PARAM_NUM = 12,
    MAX_SCRIPT_REENTRANCE = 3
};

///////////////////////////////////////////////////////////////////////////////
// Console command structure.
///////////////////////////////////////////////////////////////////////////////

typedef struct tasgSCONSOLECOMMAND
{
    // The command name.
    CHAR* szName;

    // Command arguments list.
    CHAR* szArgumentList;

    // Command help
    CHAR* szHelp;

    // The command callback function
    VOID  (*fnFunction)(CHAR* szArguments);

} SCONSOLECOMMAND, *PSCONSOLECOMMAND;


///////////////////////////////////////////////////////////////////////////////
// Console command handler
///////////////////////////////////////////////////////////////////////////////

typedef struct tasgSCONSOLEEXTENSION
{
    // 
    BOOL  (*fnExtendedHandler)(CHAR* szCommand, CHAR* szArguments);
    
    struct tasgSCONSOLEEXTENSION * psNext; 

} SCONSOLEEXTENSION, *PSCONSOLEEXTENSION;

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

typedef struct
{
    SCONSOLECOMMAND** scanCmdList;
    SCONSOLECOMMAND* currScanCmd;
    char* searchStr;
    int searchStrLen;

    char* argAssistStr;
    char* retArgStr;
    int argAssistPos;

    bool isStartedTabComplete;

    char* argAssistStrAlloc;
    char* fileAssistStrAlloc;

} STabCompleteCtx;

typedef struct
{
    char formatStr[PROMPT_FMT_STR_MAX_LEN + 1];
    char* outStr;
    bool isInlined;
    bool isConst;
} SPromptCtx;


BOOL
consoleInit( void );

VOID
consoleActivate( void );

BOOL
consoleRegisterFunctions(PSCONSOLECOMMAND psList);

VOID
consoleParse(CHAR* szCommand);

VOID
consolePreParse(CHAR* szCommand);

EKeyType
consoleCommandGet( VOID );

VOID
consolePrompt(CHAR* szParam);

VOID
consoleDump(CHAR* szParam);


PSCONSOLEEXTENSION
consoleRegisterExtension(BOOL (*fnExtendedHandler)(CHAR*, CHAR*));

 BOOL
consoleUnregisterExtension(PSCONSOLEEXTENSION psExtension);

#ifdef __cplusplus
}
#endif

#endif // end of __CONSOLE_H

