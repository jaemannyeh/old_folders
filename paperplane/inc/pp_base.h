
#ifndef _PP_BASE_H_
#define _PP_BASE_H_

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************ include files */

#include <pthread.h>

/* version information */	

#define PP_VERSION_MAJOR  0
#define PP_VERSION_MINOR  0
#define PP_VERSION_PATCH  0

/* */

#if defined(__BYTE_ORDER) && (__BYTE_ORDER == __LITTLE_ENDIAN)
#elif defined(__BYTE_ORDER) && (__BYTE_ORDER == __BIG_ENDIAN)
#error big endian unsupported
#endif

#if defined(__CYGWIN__)
#else
#endif

/* safe types */

typedef char              pp_int8_t;
typedef short             pp_int16_t;
typedef int               pp_int32_t;
typedef unsigned char     pp_uint8_t;
typedef unsigned short    pp_uint16_t;
typedef unsigned int      pp_uint32_t;

/* */

#define PP_MAX(x,y) ((x)>(y) ? (x):(y))
#define PP_MIN(x,y) ((x)<(y) ? (x):(y))

/* */
	
#define PP_BIT_SET_BY_POS(n,b)      ( n |=  (1<<b) )
#define PP_BIT_CLR_BY_POS(n,b)      ( n &= ~(1<<b) )
#define PP_BIT_GET_BY_POS(n,b)    ( ( n &   (1<<b) ) ? 1 : 0 )

/* */

int pp_printf(const char *format,...);
void pp_memory_dump16(unsigned short * adrs);

/* pp_ring */

/* pp_slist */

/* pp_mq */

/* pp_ht */

/* pp_name */

void pp_name_put(char *name,void *value);

/* pp_log */

#define PP_LOG_ERROR 2
#define PP_LOG_TRACE 4
#define PP_LOG_DEBUG 6

#ifndef PP_LOG_LEVEL
#define PP_LOG_LEVEL(x) (1)
#endif

void pp_log_set_hook(void (*func)());
void pp_log(int priority, const char *format,
            int a,int b,int c,int d,int e,int f);

#define PP_LOG(x,format,a,b,c,d,e,f) if (PP_LOG_LEVEL(PP_LOG_ ## x)) pp_log(PP_LOG_ ## x,format,a,b,c,d,e,f);

/* pp_thread & pp_timer */

void pp_thread_delay(int msec);
void pp_thread_panic();
void pp_thread_create(void (*routine)());
int pp_timer_cancel(timer_t timerid);
int pp_timer_connect(timer_t timerid,void (*routine)(),int arg);
int pp_timer_create(clockid_t clock_id,struct sigevent * evp,timer_t * pTimer);
int pp_timer_delete(timer_t timerid);
int pp_timer_gettime(timer_t timerid, struct itimerspec * value) ;
int pp_timer_settime(timer_t timerid,int flags,const struct itimerspec * value,struct itimerspec * ovalue);

#include "pp_config.h" /* pp library build configuration */

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _PP_BASE_H_ */

