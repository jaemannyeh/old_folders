/* - */

/*
 * Copyright (c)
 */

/*
modification history
--------------------
*/

/*
DESCRIPTION
                        
INCLUDE FILES:

SEE ALSO:
*/

/* includes */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/unistd.h>
#include <sys/ioctl.h> /* FIONBIO */
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

int udp1k_port = 32767;

static int udp1k_pps;
static int udp1k_bps;

static void udp1k_recv()
{
    int                   sock1k;
    struct sockaddr_in    localaddr;     
    struct sockaddr_in    fromaddr;     
    int                   addrlen;
    char                  line[1600];
    int                   bytesreceived;
    int                   fionbio_on;

    pp_log(0,"%s(%d)",__FUNCTION__,__LINE__,3,4,5,6);

    while (1)
    {
        pp_thread_delay(1000);

        sock1k = socket(AF_INET,SOCK_DGRAM,0);

        fionbio_on = 1;
        ioctl(sock1k,FIONBIO,(int)&fionbio_on);

        localaddr.sin_family      = AF_INET;     
        localaddr.sin_addr.s_addr = htonl(INADDR_ANY);    
        localaddr.sin_port        = htons(udp1k_port);

        if (-1 == bind(sock1k,(struct sockaddr *)&localaddr,sizeof(localaddr)))
        {
            close(sock1k);
            pp_log(0,"%s(%d) bind fail",__FUNCTION__,__LINE__,3,4,5,6);
        }

        while (1)
        {
            pp_thread_delay(1000);

            addrlen = sizeof(fromaddr);

            bytesreceived = recvfrom(sock1k,line,sizeof(line),0,(struct sockaddr *)&fromaddr,&addrlen);
            if (bytesreceived > 0)
            {
                udp1k_pps++;    
                udp1k_bps += bytesreceived;
            }
            pp_log(0,"%s(%d) received %d bytes",__FUNCTION__,__LINE__,bytesreceived,4,5,6);
        }

        close(sock1k);
    }
}

static void udp1k_send_1sec()
{
    int                   sock1k;
    struct sockaddr_in    toaddr;     
    char                  line[1600];
    int                   bytessent;

    pp_log(0,"%s(%d)",__FUNCTION__,__LINE__,3,4,5,6);

    while (1)
    {
        pp_thread_delay(1000);

        sock1k = socket(AF_INET,SOCK_DGRAM,0);

        toaddr.sin_family      = AF_INET;     
        toaddr.sin_addr.s_addr = htonl(inet_addr("127.0.0.1"));    
        toaddr.sin_addr.s_addr = inet_addr("127.0.0.1");    
        toaddr.sin_port        = htons(udp1k_port);

        while (1)
        {
            pp_thread_delay(1000);

            bytessent = sendto(sock1k,line,1024,0,(struct sockaddr *)&toaddr,sizeof(toaddr));
            pp_log(0,"%s(%d) sent %d bytes",__FUNCTION__,__LINE__,bytessent,4,5,6);
        }

        close(sock1k);
    }
}

static void udp1k_start()
{
    pp_thread_create(udp1k_recv);

    pp_thread_create(udp1k_send_1sec);
}

static void udp1k_report()
{
    udp1k_pps = 0;
    udp1k_bps = 0;
    pp_thread_delay(1000);

    printf("udp1k pps %d throughput %d Kbps\n",udp1k_pps,udp1k_bps*8/1024);
}

void udp1k_init()
{
    pp_name_put("udp1k_start",udp1k_start);

    pp_name_put("udp1k_report",udp1k_report);
}

