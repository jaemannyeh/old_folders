/* pp_thread.c - */

/************************************************************ include files */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#include "pp_base.h"

/****************************************************************** defines */

#define PP_TIMER_NUM          64

#if defined(PP_CYGWIN)
#define PP_TIMER_RESOLUTION   1000 /* the # of ticks per second is 1 */
#else
#define PP_TIMER_RESOLUTION   20   /* the # of ticks per second is 50 */
#endif

#define PP_TIMER_CANCELED     (-11)  /* 0xfffffff5 */
#define PP_TIMER_CONNECTED    (-12)  /* 0xfffffff4 */
#define PP_TIMER_CREATED      (-13)  /* 0xfffffff3 */
#define PP_TIMER_DELETED      (-14)  /* 0xfffffff2 */
#define PP_TIMER_ARMED        (-15)  /* 0xfffffff1 */
#define PP_TIMER_DISARMED     (-16)  /* 0xfffffff0 */

/******************************************************************** types */

/************************************************ extern & local prototypes */

/********************************************************* extern & globals */

/******************************************************************* locals */

static unsigned int pp_timer_tick = 0;

static int          pp_timer_lap[PP_TIMER_NUM];
static int          pp_timer_cnt[PP_TIMER_NUM];
static void       (*pp_timer_rtn[PP_TIMER_NUM])();
static int          pp_timer_arg[PP_TIMER_NUM];

/*============================================================ pp_thread_delay
 * description:
 * parameters:
 * returns:
 */

void pp_thread_delay(int msec)
{
#if defined(PP_CYGWIN)
    sleep(msec/1000);	
#else
    struct timespec nanopause;

    nanopause.tv_sec = msec / 1000;
    nanopause.tv_nsec = 1000000 * (msec % 1000);

    nanosleep(&nanopause,0);
#endif
}

/*============================================================ pp_thread_panic
 * description:
 * parameters:
 * returns:
 */

void pp_thread_panic()
{
    sleep(24*60*60);	
}

/*============================================================
 * description:
 * parameters:
 * returns:
 */

void pp_thread_create(void (*routine)())
{
    pthread_t thread;

    pthread_create( &thread, NULL, routine, (void*)NULL );
}

/*============================================================== pp_foreground
 * description:
 * parameters:
 * returns:
 */

void pp_foregroud()
{
}

/*============================================================== pp_background
 * description:
 * parameters:
 * returns:
 */

void pp_background()
{
    /* linux daemon writing howto devin watson */
        
    /* our process id and session id */
    pid_t pid;
    pid_t sid;
        
    /* fork off the parent process */
    pid = fork();
    if (pid < 0)
    {
        pp_printf("%s(%d)\n",__FUNCTION__,__LINE__);
        exit(-1); /* error */
    }
    /* if we got a good pid, then we can exit the parent process. */
    if (pid > 0)
    {
        pp_printf("%s(%d)\n",__FUNCTION__,__LINE__);
        exit(0); /* success */
    }

    /* change the file mode mask */
    umask(0);
            
    /* open any logs here */        
            
    /* create a new sid for the child process */
    sid = setsid();
    if (sid < 0)
    {
        pp_printf("%s(%d)\n",__FUNCTION__,__LINE__);
        exit(-1); /* error */
    }
    
#if 0
    /* Change the current working directory */
    if ((chdir("/tmp")) < 0)
    {
        pp_printf("%s(%d)\n",__FUNCTION__,__LINE__);
        exit(-1);
    }
    
    /* Close out the standard file descriptors */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    
    /* Daemon-specific initialization goes here */
    
    /* The Big Loop */
    while (1) {
       /* Do some task here ... */
       
       sleep(30); /* wait 30 seconds */
    }

    exit(0); /* success */
#endif
}

/*============================================================
 * description:
 * parameters:
 * returns:
 */

static void pp_timer_tick_handler()
{
    int timer_ix;

    pp_timer_tick++;

    for (timer_ix=(0+1); timer_ix<PP_TIMER_NUM; timer_ix++)
    {
        if (pp_timer_cnt[timer_ix] < 0)
        {
            continue;
        }

        pp_timer_cnt[timer_ix]--;

        if (pp_timer_cnt[timer_ix] == 0)
        {
            /* must be non-blocking */
            (*pp_timer_rtn[timer_ix])(timer_ix,pp_timer_arg[timer_ix]);
            pp_timer_cnt[timer_ix] = pp_timer_lap[timer_ix];
        }
    }
}

/*============================================================
 * description:
 * parameters:
 * returns:
 */

static void *pp_timer_loop(void *ptr)
{
    while (1)
    {
        pp_thread_delay(PP_TIMER_RESOLUTION);

        pp_timer_tick_handler();
    }

    return NULL;
}

/*============================================================
 * description:
 * parameters:
 * returns:
 */

static void pp_timer_init()
{
    static int pp_timer_initialized = 0;
    int        timer_ix;

    if (pp_timer_initialized != 0)
        return;

    for (timer_ix=(0+1); timer_ix<PP_TIMER_NUM; timer_ix++)
    {
        pp_timer_lap[timer_ix] = PP_TIMER_DISARMED;
        pp_timer_cnt[timer_ix] = PP_TIMER_DELETED;
        pp_timer_rtn[timer_ix] = NULL;
        pp_timer_arg[timer_ix] = 0;
    }	

    pp_thread_create(pp_timer_loop);

    pp_timer_initialized = 1;
}

/*============================================================
 * description:
 * parameters:
 * returns:
 */

int pp_timer_cancel(timer_t timerid)
{
    int timer_ix = (int)timerid;

    if (timer_ix < (0+1) || timer_ix > (PP_TIMER_NUM-1))
    {
        return -1; /* error */
    }

    pp_timer_cnt[timer_ix] = PP_TIMER_CANCELED;
	
    return 0; /* success */	
}

/*============================================================
 * description:
 * parameters:
 * returns:
 */

int pp_timer_connect(timer_t timerid,void (*routine)(),int arg)
{
    int timer_ix = (int)timerid;

    if (timer_ix < (0+1) || timer_ix > (PP_TIMER_NUM-1))
    {
        return -1; /* error */
    }

    pp_timer_cnt[timer_ix] = PP_TIMER_CONNECTED;
    pp_timer_rtn[timer_ix] = routine;
    pp_timer_arg[timer_ix] = arg;

    return 0; /* success */	
}

/*============================================================
 * description:
 * parameters:
 * returns:
 */

int pp_timer_create(clockid_t clock_id,struct sigevent * evp,timer_t * pTimer)
{
    int        timer_ix;

    pp_timer_init();

    if (evp != NULL)
    {
        pp_printf("%s(%d)\n",__FUNCTION__,__LINE__);
        return -1; /* error */
    }

    for (timer_ix=(0+1); timer_ix<PP_TIMER_NUM; timer_ix++)
    {
        if (pp_timer_cnt[timer_ix] == PP_TIMER_DELETED)
        {
            break;
        }
    }

    if (timer_ix == PP_TIMER_NUM)
    {
        pp_printf("%s(%d)\n",__FUNCTION__,__LINE__);
        return -1; /* error */
    }

    pp_timer_cnt[timer_ix] = PP_TIMER_CREATED;
    pp_timer_rtn[timer_ix] = NULL;
    pp_timer_arg[timer_ix] = 0;

    *pTimer = (timer_t)timer_ix;

    return 0; /* success */	
}

/*============================================================
 * description:
 * parameters:
 * returns:
 */

int pp_timer_delete(timer_t timerid)
{
    int timer_ix = (int)timerid;

    if (timer_ix < (0+1) || timer_ix > (PP_TIMER_NUM-1))
    {
        return -1; /* error */
    }

    pp_timer_cnt[timer_ix] = PP_TIMER_DELETED;
    pp_timer_rtn[timer_ix] = NULL;
    pp_timer_arg[timer_ix] = 0;

    return 0; /* success */	
}

/*============================================================
 * description:
 * parameters:
 * returns:
 */

int pp_timer_gettime(timer_t timerid, struct itimerspec * value) 
{
    int timer_ix = (int)timerid;
    int it_value_ms;

    if (timer_ix < (0+1) || timer_ix > (PP_TIMER_NUM-1))
    {
        return -1; /* error */
    }

    if (pp_timer_cnt[timer_ix] < 0)
    {
        it_value_ms = 0;
    }
    else
    {
        it_value_ms = pp_timer_cnt[timer_ix] * PP_TIMER_RESOLUTION;
    }

    value->it_value.tv_sec = it_value_ms / 1000;
    value->it_value.tv_nsec = (it_value_ms - (it_value_ms/1000)*1000) * 1000000;

    return 0; /* success */	
}

/*============================================================
 * description:
 * parameters:
 * returns:
 */

int pp_timer_settime(timer_t timerid,int flags,const struct itimerspec * value,struct itimerspec * ovalue)
{
    int timer_ix = (int)timerid;
    int it_value_ms;
    int it_interval_ms;

    if (timer_ix < (0+1) || timer_ix > (PP_TIMER_NUM-1))
    {
        return -1; /* error */
    }

    if (flags != 0)
    {
        return -1; /* error */
    }

    if (value == NULL)
    {
        return -1; /* error */
    }

    it_value_ms     = value->it_value.tv_sec * 1000;
    it_value_ms    += value->it_value.tv_nsec / 1000000;
    it_interval_ms  = value->it_interval.tv_sec * 1000;
    it_interval_ms += value->it_interval.tv_nsec / 1000000;

    if (it_value_ms < 1000)
    {
        it_value_ms = 1000;
    }

    if (0 < it_interval_ms && it_interval_ms < 1000) /* if (it_interval_ms < 100) */
    {
        it_interval_ms = 1000;
    }

    if (pp_timer_rtn[timer_ix] == NULL)
    {
        return -1; /* error */
    }

    pp_timer_cnt[timer_ix] = it_value_ms / PP_TIMER_RESOLUTION;

    if (it_interval_ms == 0)
    {
        pp_timer_lap[timer_ix] = PP_TIMER_DISARMED;
    }
    else
    {
        pp_timer_lap[timer_ix] = it_interval_ms / PP_TIMER_RESOLUTION;
    }

    if (ovalue != NULL)
    {
        ovalue->it_value.tv_sec = 0;
        ovalue->it_value.tv_nsec = 0;
        ovalue->it_interval.tv_sec = 0;
        ovalue->it_interval.tv_nsec = 0;
    }
	
    return 0; /* success */	
}

/*============================================================
 * description:
 * parameters:
 * returns:
 */

unsigned int pp_timer_get(unsigned int base_msec)
{
    unsigned int pp_timer_msec;

    pp_timer_init();

    pp_timer_msec = pp_timer_tick * PP_TIMER_RESOLUTION;

    if (pp_timer_msec > base_msec)
    {
        pp_timer_msec -= base_msec;
    }
    else
    {
        pp_timer_msec += ( ((unsigned int)0xffffffff) - base_msec );
    }

    return pp_timer_msec;
}

/*============================================================
 * description:
 * parameters:
 * returns:
 */

static void pp_timer_test_routine(int arg)
{
    time_t      the_current_calendar_time_of_the_system;
    time_t      current;

    the_current_calendar_time_of_the_system = time(&current);
    pp_printf("current calendar time   %d %d\n",the_current_calendar_time_of_the_system,current);
}

/*============================================================
 * description:
 * parameters:
 * returns:
 */

void pp_timer_test()
{
    timer_t           tx;
    int               msec;
    struct itimerspec time_to_be_set;
    struct itimerspec previous_time_set;

    pp_timer_create(0,NULL,&tx);

    pp_timer_connect(tx,pp_timer_test_routine,0);

    msec = 4000;

    time_to_be_set.it_value.tv_sec = msec/1000;
    time_to_be_set.it_value.tv_nsec = (msec - (msec/1000)*1000) * 1000000;

    time_to_be_set.it_interval.tv_sec = msec/1000;
    time_to_be_set.it_interval.tv_nsec = (msec - (msec/1000)*1000) * 1000000;

    pp_timer_settime(tx,0,&time_to_be_set,&previous_time_set);
}

