
#include "bs_var.h"
#include "bs_log.h"
#include "bs_tmo.h"

static int bs_initialized = 0;

void bs_init()
{
    if (bs_initialized)
        return;

    bs_initialized = 1;

    bs_var_init();
    bs_log_init();
    bs_tmo_init();
}

