


#include "bs_log.h"
#include "bs_var.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sockLib.h>

#define BS_LOG_PRIORITY BS_LOG_DBG
#define BS_LOG_FACILITY BS_LOG_CIS     /* BS_LOG_TFTPC not defined in bs_log.h */

#define BS_TFTP_PORT           69

#define BS_TFTP_MAX_PACKET     512

#define BS_TFTP_OPCODE_RRQ     1
#define BS_TFTP_OPCODE_WRQ     2
#define BS_TFTP_OPCODE_DATA    3
#define BS_TFTP_OPCODE_ACK     4
#define BS_TFTP_OPCODE_ERR     5

typedef struct bs_tftpc_data_s
{
    struct sockaddr_in peer_addr;
    unsigned short     opcode;
    int                binary;
    int                verbose;
    unsigned int       nbytes;
    int                dirlen;
    char               file[256];
} bs_tftpc_data_t;

static int bs_tftpc_receive_file(const char *dst, bs_tftpc_data_t *bs_tftpc)
{
    int sock;

    sock = socket(AF_INET,SOCK_DGRAM,0);
    if (sock == -1)
    {
        BS_LOG0(ERR,"socket() failed");
        return -1;
    }

    close(sock);

    return -1;
}

int bs_tftpc_get(const char *server,const char *srcfile,const char *dstfile,int binary,int verbose)
{
    bs_tftpc_data_t *bs_tftpc;
    int               ret;

    bs_tftpc = malloc(sizeof(*bs_tftpc));
    if (bs_tftpc == NULL)
    {
        BS_LOG0(ERR,"malloc() failed");
        return -1;
    }

    bs_tftpc->opcode = BS_TFTP_OPCODE_RRQ;
    bs_tftpc->binary = binary;
    bs_tftpc->verbose = verbose;

    ret = bs_tftpc_receive_file(dstfile,bs_tftpc);
    if (ret == 0)
    {
    }

    free(bs_tftpc);

    return -1;
}

int bs_tftpc_start()
{
    return -1;
}

const char *bs_tftpc_version()
{
    return NULL;
}
