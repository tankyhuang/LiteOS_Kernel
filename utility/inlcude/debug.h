#ifndef __DEBUG__H__
#define __DEBUG__H__

// Determine if a C++ compiler is being used.  If so, ensure that standard
// C is used to process the API information.
#ifdef __cplusplus
extern "C" {
#endif

// Port specifc file.
#include "los_typedef.h"

#define DEBUG_ENABLE 1

#define printf dbgPrint


#define ESC_COLOR_NORMAL    "\033[m"
#define ESC_COLOR_RESET     "\033[0m"
#define ESC_COLOR_BLACK     "\033[30m"
#define ESC_COLOR_RED       "\033[31m"
#define ESC_COLOR_GREEN     "\033[32m"
#define ESC_COLOR_YELLOW    "\033[33m"
#define ESC_COLOR_BLUE      "\033[34m"
#define ESC_COLOR_MAGENTA   "\033[35m"
#define ESC_COLOR_CYAN      "\033[36m"
#define ESC_COLOR_WHITE     "\033[37m"


#define NORMAL( x )         ESC_COLOR_NORMAL x
#define BLACK( x )          ESC_COLOR_BLACK  x ESC_COLOR_NORMAL
#define MAGENTA( x )        ESC_COLOR_MAGENTA x ESC_COLOR_NORMAL
#define RED( x )            ESC_COLOR_RED    x ESC_COLOR_NORMAL
#define YELLOW( x )         ESC_COLOR_YELLOW x ESC_COLOR_NORMAL
#define GREEN( x )          ESC_COLOR_GREEN  x ESC_COLOR_NORMAL
#define BLUE( x )           ESC_COLOR_BLUE   x ESC_COLOR_NORMAL
#define CYAN( x )           ESC_COLOR_CYAN   x ESC_COLOR_NORMAL
#define WHITE( x )          ESC_COLOR_WHITE  x ESC_COLOR_NORMAL

#define printf_color(color, str)    printf( color "%s" ESC_COLOR_NORMAL "\r", str )
#define fprintf_color(color, str)   fprintf( stderr, color "%s" ESC_COLOR_NORMAL "\r", str )

#define printf_red(str)             printf_color(  ESC_COLOR_RED,    str )
#define fprintf_red(str)            fprintf_color( ESC_COLOR_RED,    str )
#define printf_blue(str)            printf_color(  ESC_COLOR_BLUE,   str )
#define fprintf_blue(str)           fprintf_color( ESC_COLOR_BLUE,   str )
#define printf_green(str)           printf_color(  ESC_COLOR_GREEN,  str )
#define fprintf_green(str)          fprintf_color( ESC_COLOR_GREEN,  str )
#define printf_yellow(str)          printf_color(  ESC_COLOR_YELLOW, str )
#define fprintf_yellow(str)         fprintf_color( ESC_COLOR_YELLOW, str )

void debugInit(void);

VOID
blockPrintf( CHAR* szText, UINT16 uwLength );

VOID
dbgPrint( const CHAR* szFmt, ...);

#ifdef __cplusplus
}
#endif

#endif // end of __DEBUG__H__

