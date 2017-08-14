
#ifndef BS_TMO_H
#define BS_TMO_H

#include "bs_list.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct bs_tmo_s
{
    bs_list_t      list;      /* List header should be first member. */
    unsigned int   lap;       /* internal 'lap' counter. */
    unsigned int   msec;      /* Original millisecond timeout. */
    int            (*func)();
} bs_tmo_t;

void bs_tmo_init();
void bs_tmo_show();

#ifdef __cplusplus
}
#endif

#endif /* BS_TMO_H */

