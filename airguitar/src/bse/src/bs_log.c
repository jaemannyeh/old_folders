
#include "bs_os.h"
#include "bs_var.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sockLib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

#define BS_LOG_SIZE            512                /* 64       128        256         512 */
#define BS_LOG_SIZE_MASK       (BS_LOG_SIZE - 1)  /* 111111 1111111 11111111 111111111 */

#define BS_LOG_MAX_LINE_SIZE   72

typedef struct bs_log_data_s
{
    char                suppress;
    char                unused1;
    char                unused2;
    char                unused3;

    unsigned int        slot;
    unsigned int        lost;
    char                lines[BS_LOG_SIZE][BS_LOG_MAX_LINE_SIZE];
} bs_log_data_t;

static bs_log_data_t bs_log_data;

void bs_log_task()
{
    for (;;)
    {
        bs_sleep_msec(2000);
    }
}

void bs_log_init()
{
    memset(&bs_log_data,0,sizeof(bs_log_data));

    bs_task_create(bs_log_task);
}

static void bs_vlog(unsigned int priority, const char *format, va_list ap)
{
    int  msec;
    int  sec;
	int  len;

    msec  = bs_get_msec(0);
    sec   = msec/1000;
    msec %= 1000;

    sprintf(&bs_log_data.lines[bs_log_data.slot][0],"%07d",sec);
	bs_log_data.lines[bs_log_data.slot][7]  = '.';
    sprintf(&bs_log_data.lines[bs_log_data.slot][8],"%03d",msec);

	bs_log_data.lines[bs_log_data.slot][11] = ' ';
	bs_log_data.lines[bs_log_data.slot][12] = 0x30 + priority;
	bs_log_data.lines[bs_log_data.slot][13] = ' ';

    len = vsnprintf(&bs_log_data.lines[bs_log_data.slot][14], BS_LOG_MAX_LINE_SIZE, format, ap);

    bs_log_data.slot++;

    bs_log_data.slot &= BS_LOG_SIZE_MASK;

	*bs_log_data.lines[bs_log_data.slot] = 0;
}

void bs_log(unsigned int priority, const char *format, ...)
{
    va_list   ap;

    if (bs_log_data.suppress)
    {
        bs_log_data.lost++;
        return;
    }

    if (0 != bs_mutex_lock(NULL))
    {
        bs_log_data.lost++;
        return;
    }

    va_start(ap, format);

    bs_vlog(priority, format, ap);

    va_end(ap);

    bs_mutex_unlock(NULL);
}

void bs_log_show(int num, char *match)
{
    time_t   raw_time;
    char     buf[64];
    size_t   buf_len = sizeof(buf);
    int      slot;

    time( &raw_time );
#if defined(_WRS_KERNEL) 
    ctime_r(&raw_time,buf,&buf_len);
    buf[24] = 0;
#else
    ctime_r(&raw_time,buf);
#endif

    bs_log(0,"log slot %d log lost %d at %s",bs_log_data.slot,bs_log_data.lost,buf);

    bs_log_data.suppress = 1;
    bs_sleep_msec(10);

    slot = (int)bs_log_data.slot - num;
    if (slot < 0)
        slot += BS_LOG_SIZE;

    do
    {
        if (*bs_log_data.lines[slot])
        {
            if (match)
            {
                if (strstr(bs_log_data.lines[slot],match))
					printf("%s\n",bs_log_data.lines[slot]);
            }
            else
            {
				printf("%s\n",bs_log_data.lines[slot]);
            }
        }
        slot++;
        slot &= BS_LOG_SIZE_MASK;
    } while (slot != (int)bs_log_data.slot);

    bs_log_data.suppress = 0;
}

void bs_log_match(char *match)
{
	bs_log_show(0,match);
}




