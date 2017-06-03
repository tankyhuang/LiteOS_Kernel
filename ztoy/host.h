#ifndef __HOST_H
#define __HOST_H

// Determine if a C++ compiler is being used.  If so, ensure that standard
// C is used to process the API information.
#ifdef __cplusplus
extern "C" {
#endif


#define HM_INVALID                          0x0000
#define HM_USER                             0x0100
#define HM_CREATE                           0x0500
#define HM_NEW_MODE                         0x0600



// Determine if a C++ compiler is being used.  If so, ensure that standard
// C is used to process the API information.
#ifdef __cplusplus
}
#endif

#endif
