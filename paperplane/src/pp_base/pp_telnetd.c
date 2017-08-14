/* pp_telnetd.c - */

/************************************************************ include files */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/unistd.h>
#include <sys/ioctl.h>     /* for FIONBIO */
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#include "pp_base.h"

/****************************************************************** defines */

#define DATA_MARK            242
#define BREAK                243
#define INTERRUPT_PROCESS    244
#define ARE_YOU_THERE        246
#define WILL                 251
#define WONT                 252
#define DO                   253
#define DONT                 254
#define INTERPRET_AS_COMMAND 255

#define STATE_DATA_MARK               DATA_MARK
#define STATE_WILL                    WILL
#define STATE_WONT                    WONT
#define STATE_DO                      DO
#define STATE_DONT                    DONT
#define STATE_INTERPRET_AS_COMMAND    INTERPRET_AS_COMMAND

/******************************************************************** types */

/************************************************ extern & local prototypes */

/********************************************************* extern & globals */

/******************************************************************* locals */

#ifdef PP_LOG
    static int pp_telnetd_log_level = PP_LOG_DEBUG;
    #undef  PP_LOG_LEVEL
    #define PP_LOG_LEVEL(x)    (pp_telnetd_log_level >= x)
#else
    #define PP_LOG             do {} while(0)    
#endif

static int     pp_telnetd_port = PP_TELNETD_PORT;
static int     pp_telnetd_sock;
static int     pp_telnetd_client_sock;

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

static int pp_telnetd_write(char *buf,size_t n)
{
    write(pp_telnetd_client_sock,buf,n);
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

static int pp_telnetd_state(int state,char *line,int len)
{
    int           lx;
    unsigned char cx;

    for (lx=0; lx<len; lx++)
    {
        cx = line[lx];

        switch (state)
        {
        case STATE_DATA_MARK:
            if (cx == INTERPRET_AS_COMMAND)
            {
                state = STATE_INTERPRET_AS_COMMAND;
            }
            else
            {
            }
            break;
        case STATE_INTERPRET_AS_COMMAND:
            switch (cx)
            {
            case BREAK:
                state = STATE_DATA_MARK;
                break;
            case INTERRUPT_PROCESS:
                state = STATE_DATA_MARK;
                break;
            default:
                state = STATE_DATA_MARK;
                break;
            }

            break;
        default:
            state = STATE_DATA_MARK;
            break;
        }    
    }

    pp_shell(line);

    return state;
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

static int pp_telnetd_client_is_readable(int fd, int seconds)
{
    struct timeval   tv;
    fd_set           read_fds;

    tv.tv_sec = seconds;
    tv.tv_usec = 0;

    FD_ZERO( &read_fds );
    FD_SET( fd, &read_fds );

    if (select( fd+1, &read_fds, NULL, NULL, &tv ) <= 0)
        return 0; /* false */
    else
        return 1; /* true */
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

static void pp_telnetd_client()
{
    char               line[128];
    int                len;
    int                state = STATE_DATA_MARK;

    pp_printf_set_write( pp_telnetd_write );

    while (1)
    {
        if (pp_telnetd_client_is_readable(pp_telnetd_client_sock,60) == 0)
        {
            pp_printf("%s(%d) timed out after 60 seconds.\n",__FUNCTION__,__LINE__);
            break;
        }

        memset(line,0,sizeof(line));

        len = read(pp_telnetd_client_sock,line,sizeof(line));
        if (len <= 0)
        {
            break;
        }

        state = pp_telnetd_state(state,line,len);

        write(pp_telnetd_client_sock, "] ", 2);
    }

    pp_printf_set_write( NULL );
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

static void pp_telnetd_accept()
{
    struct sockaddr_in    sin;
    int                   length;

    while (1)
    {
        pp_thread_delay(1000);

        memset((char *)&sin,0,sizeof(sin));

        length = sizeof(sin);     
        pp_telnetd_client_sock = accept(pp_telnetd_sock, (struct sockaddr *)&sin, &length); 
        if (pp_telnetd_client_sock == -1)
            break;

        PP_LOG(TRACE,"%s(%d) 0x%08x:%d",__FUNCTION__,__LINE__,ntohl(sin.sin_addr.s_addr),ntohs(sin.sin_port),5,6);

        pp_telnetd_client();

        if (pp_telnetd_client_sock != -1)
        {
            close(pp_telnetd_client_sock);
            pp_telnetd_client_sock = -1;
        }
    }
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

static void *pp_telnetd_loop(void *ptr)
{
    struct sockaddr_in    sin;     

    pp_printf("%s(%d) telnet 127.0.0.1 %d\n",__FUNCTION__,__LINE__,pp_telnetd_port);

    while (1)
    {
        pp_thread_delay(1000);

        pp_telnetd_sock = socket(AF_INET,SOCK_STREAM,0);
        if (-1 == pp_telnetd_sock)
        {
            PP_LOG(ERROR,"%s(%d) socket fail",__FUNCTION__,__LINE__,3,4,5,6);
            continue;
        }

        memset((char *)&sin,0,sizeof(sin));

        sin.sin_family      = AF_INET;     
        sin.sin_port        = htons(pp_telnetd_port);
        sin.sin_addr.s_addr = htonl(INADDR_ANY);    

        if (-1 == bind(pp_telnetd_sock,(struct sockaddr *)&sin,sizeof(sin)))
        {
            PP_LOG(ERROR,"%s(%d) bind fail",__FUNCTION__,__LINE__,3,4,5,6);
            close(pp_telnetd_sock);
            pp_telnetd_sock = -1;
            continue;
        }
     
        listen(pp_telnetd_sock,2);

        pp_telnetd_accept();

        close(pp_telnetd_sock);
        pp_telnetd_sock = -1;
    }

    return NULL;
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

void pp_telnetd_init()
{
    PP_LOG(TRACE,"%s(%d)",__FUNCTION__,__LINE__,3,4,5,6);

    pp_name_put("pp_telnetd_log_level",&pp_telnetd_log_level);

    pp_name_put("pp_telnetd_port",&pp_telnetd_port);

    pp_thread_create(pp_telnetd_loop);
}

