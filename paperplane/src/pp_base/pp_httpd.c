/* pp_httpd.c - */

/************************************************************ include files */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/unistd.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#include "pp_base.h"

/****************************************************************** defines */

/******************************************************************** types */

/************************************************ extern & local prototypes */

/********************************************************* extern & globals */

/******************************************************************* locals */

#ifdef PP_LOG
    static int pp_httpd_log_level = PP_LOG_DEBUG;
    #undef  PP_LOG_LEVEL
    #define PP_LOG_LEVEL(x)    (pp_httpd_log_level >= x)
#else
    #define PP_LOG             do {} while(0)    
#endif

static int pp_httpd_port = PP_HTTPD_PORT;
static int pp_httpd_sock;
static int pp_httpd_client_sock;

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

static int pp_httpd_write(char *buf,size_t n)
{
    int   len;

    len = write(pp_httpd_client_sock,buf,n);
    if (len != n)
    {
        printf("%s(%d)\n",__FUNCTION__,__LINE__);
    }

    if (strchr(buf,0x0a))
    {
       write(pp_httpd_client_sock,"<br />",6);
    }

    return len;
}

static void httpw(char *s)
{
    pp_httpd_write(s,strlen(s));
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

static void version()
{
    char        line[128];
    time_t      current;

    current = time(NULL);
    sprintf(line,"%s",ctime(&current));
    httpw(line);

    httpw("<br />");

    sprintf(line,"%s %s",__DATE__,__TIME__);
    httpw(line);
}

typedef struct
{
    char         *name;
    void        (*func)();    
} pp_page_t;

static pp_page_t pp_pages[] = {
    { "version", version },    
    { NULL, NULL }    
};

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

static void pp_httpd_page(char *page_name)
{
    char *px;
    char *qx;

    px = strstr(page_name,"?shell=");
    if (px == NULL)
    {
        pp_page_t *page;

        page = &pp_pages[0];
        while (page->name)
        {
            if (strcmp(page->name,page_name) == 0)
            {
                (page->func)();
                break;        
            }
            page++;      
        }
    }
    else
    {
        px += strlen("?shell=");

        while (qx = strchr(px,'+'))
            *qx = ' ';
    }

    pp_shell(px);
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

static void pp_httpd_url(char *URL)
{
    httpw("<html>");    
    httpw(  "<head>");    
    httpw(    "<title>default</title>");    
    httpw(  "</head>");    
    httpw(  "<body>");    

    httpw(    "<p>");
    httpw(      "<a href='version'>version</a> ");
    httpw(    "</p>");

    httpw(    "<form method='get'>");
    httpw(      "<input type='text' name='shell' size='80'>");
    httpw(      "<input type='submit' value='submit'>");
    httpw(    "</form>");

    httpw(    "<font face='courier new'>");
    pp_httpd_page(&URL[1]);
    httpw(    "</font>");

    httpw(  "</body>");    
    httpw("</html>");    
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

static int pp_httpd_state(int state,char *line,int len)
{
    char *lasts;
    char *method;
    char *URL;

    method = strtok_r(line," ",&lasts);
    URL = strtok_r(NULL," ",&lasts);

    pp_httpd_url(URL);

    return state;
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

void pp_httpd_client()
{
    char line[1000];
    int  len;
    int  state = 0;

    pp_printf_set_write( pp_httpd_write );

    while (1)
    {
        memset(line,0,sizeof(line));

        len = read(pp_httpd_client_sock,line,sizeof(line));
        if (len <= 0)
            break;        

        state = pp_httpd_state(state,line,len);

        break;
    }        

    pp_printf_set_write( NULL );
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

void pp_httpd_accept()
{
    struct sockaddr_in    sin;     
    int                   length;

    while (1)
    {
        pp_thread_delay(1000);

        memset((char *)&sin,0,sizeof(sin));

        length = sizeof(sin);     
        pp_httpd_client_sock = accept(pp_httpd_sock, (struct sockaddr *)&sin, &length);         
        if (pp_httpd_client_sock == -1)
            break;

        PP_LOG(TRACE,"%s(%d) 0x%08x:%d",(int)__FUNCTION__,__LINE__,ntohl(sin.sin_addr.s_addr),ntohs(sin.sin_port),5,6);

        pp_httpd_client(pp_httpd_client_sock);

        if (pp_httpd_client_sock != -1)
        {
            close(pp_httpd_client_sock);
            pp_httpd_client_sock = -1;
        }
    }
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

static void *pp_httpd_loop(void *ptr)
{
    struct sockaddr_in    sin;     

    pp_printf("%s(%d) http://127.0.0.1:%d\n",__FUNCTION__,__LINE__,pp_httpd_port);

    while (1)
    {
        pp_thread_delay(1000);

        pp_httpd_sock = socket(AF_INET,SOCK_STREAM,0);
        if (-1 == pp_httpd_sock)
        {
            PP_LOG(ERROR,"%s(%d) socket fail",(int)__FUNCTION__,__LINE__,3,4,5,6);
            continue;
        }

        memset((char *)&sin,0,sizeof(sin));

        sin.sin_family      = AF_INET;     
        sin.sin_port        = htons(pp_httpd_port);
        sin.sin_addr.s_addr = htonl(INADDR_ANY);    

        if (-1 == bind(pp_httpd_sock,(struct sockaddr *)&sin,sizeof(sin)))
        {
            close(pp_httpd_sock);
            PP_LOG(ERROR,"%s(%d) bind fail",(int)__FUNCTION__,__LINE__,3,4,5,6);
            perror(__FUNCTION__);
            continue;
        }
     
        listen(pp_httpd_sock,2);

        pp_httpd_accept();

        close(pp_httpd_sock);
        pp_httpd_sock = -1;
    }
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

void pp_httpd_init()
{
    PP_LOG(TRACE,"%s(%d)",(int)__FUNCTION__,__LINE__,3,4,5,6);

    pp_name_put("pp_httpd_log_level",&pp_httpd_log_level);

    pp_name_put("pp_httpd_port",&pp_httpd_port);

    pp_thread_create(pp_httpd_loop);
}

