

#include "bs_log.h"
#include "bs_var.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sockLib.h>

#define BS_LOG_PRIORITY BS_LOG_DBG
#define BS_LOG_FACILITY BS_LOG_CIS     /* BS_LOG_TFTPS not defined in bs_log.h */

typedef struct bs_tftps_data_s
{
    int fd;
    struct sockaddr_in local_addr;
    struct sockaddr_in peer_addr;
} bs_tftps_data_t;

static bs_tftps_data_t bs_tftps;

void bs_tftps_proc()
{
    for (;;)
    {
        sleep(2);

        bs_tftps.fd = socket(AF_INET,SOCK_DGRAM,0);
        if (bs_tftps.fd == -1)
        {
            BS_LOG0(ERR,"socket() failed");
            continue;
        }


        memset(&bs_tftps.local_addr,0,sizeof(bs_tftps.local_addr));
        bs_tftps.local_addr.sin_family = AF_INET;
        bs_tftps.local_addr.sin_port = htons(0);
        bs_tftps.local_addr.sin_addr.s_addr = INADDR_ANY;
        if (bind(bs_tftps.fd,(struct sockaddr *)&bs_tftps.local_addr,sizeof(bs_tftps.local_addr)) == -1)
        {
            BS_LOG0(ERR,"bind() failed");
            goto failed;
        }


failed:
        close(bs_tftps.fd);
    }
}

int bs_tftps_start()
{
    return -1;
}

const char *bs_tftps_version()
{
    return NULL;
}
