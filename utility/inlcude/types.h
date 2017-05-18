#ifndef _TYPES_H_
#define _TYPES_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "stdio.h"
#include "stdlib.h"
#include "los_typedef.h"


//////////////////////////////////////////////////////////////////////////////
//	TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////////

typedef long             ERROR_T;

#ifndef OK
#define OK	            ((long)0)
#endif

#ifndef ERR
#define ERR	            ((long)-1)
#endif



#ifdef __cplusplus
}
#endif

#endif // _TYPES_H_
