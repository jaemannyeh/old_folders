
#include "bs_log.h"
#include "bs_var.h"

#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sockLib.h>

#define BS_LOG_PRIORITY BS_LOG_DBG
#define BS_LOG_FACILITY BS_LOG_CIS     /* BS_LOG_DHCPC not defined in bs_log.h */

typedef struct bs_dhcpc_data_s
{
    int                fd;
    struct sockaddr_in local_addr; /* local address */
} bs_dhcpc_data_t;

static bs_dhcpc_data_t bs_dhcpc;

static int bs_dhcpc_init()
{

    bs_dhcpc.fd = socket(AF_INET,SOCK_DGRAM,0);
    if (bs_dhcpc.fd == -1)
    {
        BS_LOG0(ERR,"socket() failed");
        goto leave;
    }

    bs_dhcpc.local_addr.sin_port = htons( bs_var_get_int("bs_dhcpc_client_port",68) ); /* 68 */

leave:
    return -1;
}

static void bs_dhcpc_loop()
{
    for (;;)
    {
        sleep(1);
    }
}

void bs_dhcpc_task()
{
    int ret;

    ret = bs_dhcpc_init();
    if (ret != 0)
    {
        BS_LOG1(ERR,"failed, ret=%d",ret);
        goto leave;
    }

    bs_dhcpc_loop();

leave:
    return;
}

int bs_dhcpc_start()
{
    return -1;
}

const char *bs_dhcpc_version()
{
    return NULL;
}

