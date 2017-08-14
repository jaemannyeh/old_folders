#ifndef BS_OS_H
#define BS_OS_H

#include <vxWorks.h>
#include <taskLib.h>
#include <tickLib.h>

#ifdef __cplusplus
extern "C" {
#endif

unsigned int bs_get_msec(unsigned int last_msec);

void bs_sleep_msec(unsigned int msec);

int bs_mutex_lock(void* mutex);

int bs_mutex_unlock(void* mutex);

void bs_task_create(void (*entry)());

#ifdef __cplusplus
}
#endif

#endif /* BS_OS_H */

