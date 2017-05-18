#ifndef __DEBUG__H__
#define __DEBUG__H__

// Determine if a C++ compiler is being used.  If so, ensure that standard
// C is used to process the API information.
#ifdef __cplusplus
extern "C" {
#endif

// Port specifc file.
#include "los_typedef.h"

void 
debugInit(void);

void
blockPrintf( char* str, uint16_t len );

void
dbg_print( const char* format, ...);


void
_log_d(const char *format, ...);


#define DEBUG_ENABLE 1

#if DEBUG_ENABLE
#define TRACE(...)    _log_d(__VA_ARGS__)

#ifndef ASSERT      
#define ASSERT(_x)                                                              \
        if (!(_x))                                                              \
        {                                                                       \
			dbg_error("Assert Failed\n");	    		                        \
        }
#endif


#else
#define TRACE(...)
#define ASSERT(...)
#endif


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

#define STI_ERROR_PREFIX      "[ERR]"
#define STI_WARNING_PREFIX    "[WRN]"
#define STI_INFO_PREFIX       "[INF]"
#define STI_DEBUG_PREFIX      "[DBG]"

#define STI_ERROR_FORMAT(_format)   RED(STI_ERROR_PREFIX) ": %s(%d): " _format ,    __FILE__, __LINE__
#define STI_WARN_FORMAT(_format)    RED(STI_WARNING_PREFIX) ": %s(%d): " _format ,  __FILE__, __LINE__
#define STI_INFO_FORMAT(_format)    RED(STI_ERROR_PREFIX) ": %s(%d): " _format ,    __FILE__, __LINE__
#define STI_DBG_FORMAT(_format)     RED(STI_DEBUG_PREFIX) ": %s(%d): " _format ,    __FILE__, __LINE__

#define printf           dbg_print
#define dbg_error(a,...) dbg_print(STI_ERROR_FORMAT(a), ## __VA_ARGS__)




#ifdef __cplusplus
}
#endif

#endif // end of __DEBUG__H__

