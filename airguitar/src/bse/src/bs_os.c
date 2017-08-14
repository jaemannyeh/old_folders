
#include "bs_os.h"

#include <stdio.h>
#include <sysLib.h>

unsigned int bs_get_msec(unsigned int last_msec)
{
#if defined(_WRS_KERNEL) 
    unsigned int msec;

    msec  = tickGet() * 1000 / sysClkRateGet();
    msec -= last_msec;    

    return msec;
#else
    #error _WRS_KERNEL is required
#endif

}
void bs_sleep_msec(unsigned int msec)
{
#if defined(_WRS_KERNEL) 
    int ticks = msec * sysClkRateGet() / 1000;
    if (ticks == 0)
        ticks++;
    taskDelay(ticks);
#else
    #error _WRS_KERNEL is required
#endif
}

int bs_mutex_lock(void* mutex)
{
    return 0; /* 0:success -1:fail */
}

int bs_mutex_unlock(void* mutex)
{
    return 0; /* 0:success -1:fail */
}

void bs_run_cmd(char *cmd_str)
{
#if defined(_WRS_KERNEL) 
    char  *cmd_line;

    cmd_line = strdup(cmd_str);
    if (cmd_line == NULL)
    {
        return;
    }

    free(cmd_line);

    return;
#else
    #error _WRS_KERNEL is required
#endif
}

void bs_task_create(void (*entry)())
{
#if defined(_WRS_KERNEL) 
    static int task_number = 100001;
    char task_name[16];

    sprintf(task_name,"t%d",task_number++);
    (void)taskSpawn(task_name,100,VX_FP_TASK,8000,(FUNCPTR)entry,0,0,0,0,0,0,0,0,0,0);
#else
    #error _WRS_KERNEL is required
#endif
}



