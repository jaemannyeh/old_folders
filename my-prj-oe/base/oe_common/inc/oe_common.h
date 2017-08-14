
#ifndef OE_COMMON_H
#define OE_COMMON_H

/* 1 description ******************************************/

/* 2 configuration ****************************************/

/* 3 include files ****************************************/

#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 4 defines **********************************************/

#define OE_TRUE     1
#define OE_FALSE    0

#define OE_DBGLOG(format,...)    oe_dbglog(__FILE__,__LINE__,format,__VA_ARGS__)

/* 5 types ************************************************/

typedef unsigned int          oe_u32;
typedef int                   oe_s32;

typedef unsigned short        oe_u16;
typedef short                 oe_s16;

typedef unsigned char         oe_u8;
typedef char                  oe_s8;

typedef void * oe_mutex_t;
typedef void * oe_sem_t;

/* 6 functions ********************************************/

void  oe_free(void *ptr);

oe_mutex_t oe_mutex_create();
int        oe_mutex_delete(oe_mutex_t *mutex); /* mutex will be NULL */
int        oe_mutex_lock(oe_mutex_t mutex);
int        oe_mutex_unlock(oe_mutex_t mutex);

oe_sem_t oe_sem_create();
int      oe_sem_delete(oe_sem_t *sem); /* sem will be NULL */
int      oe_sem_pend(oe_sem_t sem);
int      oe_sem_post(oe_sem_t sem);

int      oe_sleep(int seconds);
int      oe_millisleep(int milliseconds);

void         oe_restart();
void         oe_shutdown();
unsigned int oe_uptime_get(); /* in seconds */

int oe_dbglog(const char *filename,int linenum,const char *format,...);

int   oe_tagvar_int_set(const char *name,int value);
char *oe_tagvar_str_set(const char *name,const char *value);
int   oe_tagvar_int_get(const char *name,int default_value);
char *oe_tagvar_str_str(const char *name);

int oe_timeout_set();    /* set a timeout in msec */
int oe_timeout_cancel(); /* cancel a timeout */
int oe_timeout_get();    /* get msec left to timeout */

double oe_latitude_get();                      /* [-90.0, 90.0] */
double oe_longitude_get();                     /* [-180.0, 180.0] */
float  oe_altitude_get();                      /* meters */
float  oe_altitude_wrt_ellipsoid_get();        /* meters above WGS84 ellipsoid */
float  oe_altitude_wrt_mean_sea_level_get();   /* meters above mean sea level */

#ifdef __cplusplus
}
#endif

#endif /* OE_COMMON_H */

