#ifndef __FREQ__H__
#define __FREQ__H__

// Determine if a C++ compiler is being used.  If so, ensure that standard
// C is used to process the API information.
#ifdef __cplusplus
extern "C" {
#endif


#define GET_SYS_FREQ_HZ()       ( SystemCoreClock )
#define GET_SYS_FREQ_KHZ()      ( SystemCoreClock / 1000 )
#define GET_SYS_FREQ_MHZ()      ( SystemCoreClock / 1000000 )
#define GET_COUNTER_FREQ_HZ()   ( SystemCoreClock / 2 )
#define GET_COUNTER_FREQ_KHZ()  ( SystemCoreClock / 2000 )











#ifdef __cplusplus
}
#endif

#endif // end of __FREQ__H__
