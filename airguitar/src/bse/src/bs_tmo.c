
#include "bs_log.h"
#include "bs_os.h"
#include "bs_tmo.h"
#include "bs_list.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define BS_LOG_PRIORITY BS_LOG_DBG
#define BS_LOG_FACILITY BS_LOG_CIS     /* BS_LOG_TMO not defined in bs_log.h */

#define BS_TMO_RESOLUTION_MSEC 10

#define BS_TMO_SIZE   256                /* 64       128        256         */
#define BS_TMO_MASK   (BS_TMO_SIZE - 1)  /* 111111 1111111 11111111 */
#define BS_TMO_SHIFT  8                  /* 6         7           8            */

typedef struct bs_tmo_data_s
{
    bs_list_t     wheel[BS_TMO_SIZE];
    unsigned int  slot;
    bs_list_t    *next;
    unsigned int  pulse;       /* one-pulse-every-slot */
} bs_tmo_data_t;

bs_tmo_data_t bs_tmo;

void bs_tmo_task()
{
	bs_list_t    *entry;
	bs_tmo_t     *tmo;
    int           repeat;
    unsigned int  slot;

    for (;;)
    {
        bs_sleep_msec(BS_TMO_RESOLUTION_MSEC);

        bs_tmo.pulse++;

		bs_mutex_lock(NULL);

        if (++bs_tmo.slot == BS_TMO_SIZE)
            bs_tmo.slot = 0;

        entry = bs_list_first(&bs_tmo.wheel[bs_tmo.slot]);
        while (entry)
        {
            bs_tmo.next = bs_list_next(entry);
            tmo = (bs_tmo_t *)entry;

            if (tmo->lap != 0)
            {
                tmo->lap--;
            }
            else
            {
				bs_list_remove(&tmo->list);

                repeat = 0;

                if (tmo->func)
                    repeat = tmo->func();

                if (repeat)
                {
					slot	 = tmo->msec / BS_TMO_RESOLUTION_MSEC;
                    if (tmo->msec % BS_TMO_RESOLUTION_MSEC)
                        slot++;
					tmo->lap = (slot - 1) >> BS_TMO_SHIFT;
					slot	 = (bs_tmo.slot + slot) & BS_TMO_MASK;

					bs_list_insert_first(&bs_tmo.wheel[slot], &tmo->list);
                }
            }
            
            entry = bs_tmo.next;
        }

		bs_mutex_unlock(NULL);
    }
}

void bs_tmo_init()
{
    int tx;

    memset(&bs_tmo, 0, sizeof(bs_tmo));

    for (tx = 0; tx < BS_TMO_SIZE; tx++)
        bs_list_init(&bs_tmo.wheel[tx]);

    bs_task_create(bs_tmo_task);
}

void bs_tmo_set(bs_tmo_t *tmo,unsigned int msec,int(*func)())
{
    unsigned int slot;

    bs_mutex_lock(NULL);

    tmo->msec = msec;
    tmo->func = func;

    slot     = msec / BS_TMO_RESOLUTION_MSEC;
    tmo->lap = slot >> BS_TMO_SHIFT;
    slot     = (bs_tmo.slot + slot + 1) & BS_TMO_MASK;

    bs_list_insert_first(&bs_tmo.wheel[slot], &tmo->list);

    bs_mutex_unlock(NULL);
}

void bs_tmo_cancel(bs_tmo_t *tmo)
{
    bs_mutex_lock(NULL);

	bs_list_remove(&tmo->list);
 
    bs_mutex_unlock(NULL);
}

void bs_tmo_reset(bs_tmo_t *tmo)
{
    bs_tmo_cancel(tmo);
    bs_tmo_set(tmo,tmo->msec,tmo->func);
}

unsigned int bs_tmo_get_msec(bs_tmo_t *tmo)
{
    unsigned int msec;
    int slot;

    bs_mutex_lock(NULL);

    msec = tmo->lap << BS_TMO_SHIFT;

    slot = ((unsigned int)tmo->list.head - (unsigned int)bs_tmo.wheel) / sizeof(bs_list_t);
    slot = (int)slot - bs_tmo.slot;
    if (slot < 0)
        slot += BS_TMO_SIZE;
    msec += slot;

    msec *= BS_TMO_RESOLUTION_MSEC;

    bs_mutex_unlock(NULL);

    return msec;
}

void bs_tmo_show()
{
    int num_tmo; /* number of tmo */
    int slot;

    bs_tmo_init(); /* do nothing if bs_tmo_initialized is not 0. */

    bs_mutex_lock(NULL);

	num_tmo = 0;
	slot = bs_tmo.slot;
    do
    {
        num_tmo += bs_tmo.wheel[slot].size;
        slot++;
		slot &= BS_TMO_MASK;
    } while (slot != bs_tmo.slot);

    bs_mutex_unlock(NULL);

    printf("number of tmo %d, slot %d, pulse %d\n",num_tmo,bs_tmo.slot,bs_tmo.pulse);

}

