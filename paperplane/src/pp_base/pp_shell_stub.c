/* pp_shell_stub.c - */

/************************************************************ include files */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>        /* offsetof() */
#include <string.h>
#include <time.h>
#include <math.h>
#include <ctype.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>

#include <fcntl.h>
#include <sys/mman.h>

#include "pp_base.h"

/****************************************************************** defines */

/******************************************************************** types */

/************************************************ extern & local prototypes */

/********************************************************* extern & globals */

/******************************************************************* locals */

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

static volatile int *pp_stack_trace_test_two_local2;

static volatile void pp_stack_trace_test_two(volatile int arg1, volatile int arg2)
{
    volatile int local1;
    volatile int local2;

    local1 = 0x33333333;
    local2 = 0x44444444;

    pp_stack_trace_test_two_local2 = &local2;

    pp_thread_delay(3000);
}

static volatile void pp_stack_trace_test_one()
{
    pp_stack_trace_test_two(0x11111111,0x22222222);
}

static void *pp_stack_trace_test(void *ptr)
{
    printf("%s(%d) --> one() --> two()\n",__FUNCTION__,__LINE__);

    pp_stack_trace_test_one();

    printf("%s(%d) <-- one() <-- two()\n",__FUNCTION__,__LINE__);
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

static void pp_bit_array(char **array,int row,int column)
{
    pp_printf("array               0x%x\n",array);    
    pp_printf("&array[0][0]        0x%x\n",&array[0][0]);    
    pp_printf("&array[1][0]        0x%x\n",&array[1][0]);    
    pp_printf("((int)array)+column 0x%x\n",((int)array)+column);    
}

/*===================================================================== pp_bit
 * description:
 * parameters:
 * returns:
 */

void pp_bit()
{
    char * dashline = "-----------------------------------------------------------------------\n";

    pp_printf(dashline);
    if (1)
    {
        pp_printf("char       %d\n",sizeof(char));
        pp_printf("short      %d\n",sizeof(short));
        pp_printf("int        %d\n",sizeof(int));
        pp_printf("long       %d\n",sizeof(long));
        pp_printf("long long  %d\n",sizeof(long long));
        pp_printf("float      %d\n",sizeof(float));
        pp_printf("double     %d\n",sizeof(double));
        pp_printf("void *     %d\n",sizeof(void *));
        pp_printf("true       %d\n",(1==1));
        pp_printf("false      %d\n",(1==0));
    }

    pp_printf(dashline);
    if (1)
    {
        char array[2][16];
        pp_printf("array               0x%x\n",array);    
        pp_printf("&array[0][0]        0x%x\n",&array[0][0]);    
        pp_printf("&array[1][0]        0x%x\n",&array[1][0]);    
        pp_bit_array((char **)array,2,16);
    }

    pp_printf(dashline);
    if (1)
    {
        struct _sx {
            char        _char5;     /* 1        4    */
            int         _int;       /* 4  5     4 8  */
            char        _char6;     /* 1  6     4 12 */
            int         _short;     /* 2  8     4 16 */
            char        _char7;     /* 1  9     8 24 */
            long long   _longlong;  /* 8  17    8 32 */
        } sx;

        pp_printf("sizeof(sx)      %d\n",sizeof(sx));
        pp_printf("&(sx._char5)    0x%08x offset %d\n",(unsigned int)&(sx._char5),offsetof(struct _sx,_char5));
        pp_printf("&(sx._int)      0x%08x offset %d\n",(unsigned int)&(sx._int),offsetof(struct _sx,_int));
        pp_printf("&(sx._char6)    0x%08x offset %d\n",(unsigned int)&(sx._char6),offsetof(struct _sx,_char6));
        pp_printf("&(sx._short)    0x%08x offset %d\n",(unsigned int)&(sx._short),offsetof(struct _sx,_short));
        pp_printf("&(sx._char7)    0x%08x offset %d\n",(unsigned int)&(sx._char7),offsetof(struct _sx,_char7));
        pp_printf("&(sx._longlong) 0x%08x offset %d\n",(unsigned int)&(sx._longlong),offsetof(struct _sx,_longlong));

        memset(&sx,0,sizeof(sx));

        sx._char5 = 0x55;
        if (sizeof(int) == 4) sx._int = 0x12345678; /* 78563412 */
        sx._char6 = 0x66;
        if (sizeof(short) == 2) sx._short = 0x1234; /* 3412 */
        sx._char7 = 0x77;
        if (sizeof(long long) == 8) sx._longlong = 0x12345678abcdef01; /* 01efcdab78563412 */
        pp_memory_dump16( (int)&sx );
        pp_memory_dump16( 16+(int)&sx );
    }

    pp_printf(dashline);
    if (1)
    {
        float  fx;
        double dx;
        union {
            float fx;
            int   ix;
        } ux;

        fx = 3.14; dx = 3.14;
        pp_printf("3.14 = %f = %f\n",fx,dx);
        fx *= 3.14; dx *= 3.14;
        pp_printf("3.14 * 3.14  = 9.8596     = %f = %f\n",fx,dx);

        fx = pow(32.01,1.54); dx = pow(32.01,1.54);
        pp_printf("32.01 ^ 1.54 = 208.036691 = %f = %f\n",fx,dx);

        fx = sqrt(1024.0); dx = sqrt(1024.0);
        pp_printf("1024.0 ^ 0.5 = 32         = %f = %f\n",fx,dx);

        ux.fx = 3.14;              pp_printf("0x%08x %f\n",ux.ix,ux.fx);
        ux.ix = 0x00000000;        pp_printf("0x%08x %f\n",ux.ix,ux.fx);
        ux.ix = 0x0000000f;        pp_printf("0x%08x %f\n",ux.ix,ux.fx);
        ux.ix = 0xf0000000;        pp_printf("0x%08x %f\n",ux.ix,ux.fx);
        ux.ix = 0xffffffff;        pp_printf("0x%08x %f\n",ux.ix,ux.fx);
        ux.fx = sqrt(-1);          pp_printf("0x%08x %f\n",ux.ix,ux.fx);
        ux.fx = pow(3.14,314.0);   pp_printf("0x%08x %f\n",ux.ix,ux.fx);
        ux.fx = log(0);            pp_printf("0x%08x %f\n",ux.ix,ux.fx);
    }

    pp_printf(dashline);
    if (1)
    {
        void         *mx;
        unsigned int  nx = 0x80000000;

        do {
            nx >>= 1;

            mx = malloc(nx);
            if (mx != NULL)
            {
                pp_printf("max size of malloc....0x%08x\n",nx);
                pp_memory_dump16( mx );
                nx = 0;
            }

            free(mx);
        } while (nx > 0x00000001);
    }


    pp_printf(dashline);
    if (1)
    {
        int socket_array[1024];
        int sx;

        for (sx=0; sx<sizeof(socket_array)/sizeof(int); sx++)
        {
            socket_array[sx] = -1;
        }

        for (sx=0; sx<sizeof(socket_array)/sizeof(int); sx++)
        {
            socket_array[sx] = socket(AF_INET,SOCK_STREAM,0);
            if (socket_array[sx] == -1)
                break;
        }

        sx -= 1;

        printf("min of socket descriptor %4d %d\n",0,socket_array[0]);
        printf("max of socket descriptor %4d %d\n",sx,socket_array[sx]);

        for (sx=0; sx<sizeof(socket_array)/sizeof(int); sx++)
        {
            close( socket_array[sx] );
        }
    }

    pp_printf(dashline);
    if (1)
    {
        void *mq;        
    char message[16];

        mq = pp_mq_open(4,16);

    pp_mq_send(mq,"one",3);
    pp_mq_send(mq,"two",3);
    pp_mq_send(mq,"three",5);
    pp_mq_send(mq,"four",4);

    pp_mq_receive(mq,message,16); pp_printf("%c\n",message[0]);
    pp_mq_receive(mq,message,16); pp_printf("%c\n",message[0]);
    pp_mq_receive(mq,message,16); pp_printf("%c\n",message[0]);
    pp_mq_receive(mq,message,16); pp_printf("%c\n",message[0]);

    pp_mq_show(mq);
    }

    pp_printf(dashline);
    if (1)
    {
        const char* str0 = "Watchmen";
        const char* str1 = "Watchmen";
        char*       str2 = "Watchmen";
        char*       str3 = "Watchmen";
        const char* str4 = "atchmen"; 
        char*       str5 = "tchmen"; 
        char*       str6 = "chmen";

        pp_memory_dump16( (0xfffffff0&(int)str0)+0x00 );
        pp_memory_dump16( (0xfffffff0&(int)str0)+0x10 );
        pp_memory_dump16( (0xfffffff0&(int)str0)+0x20 );
        pp_memory_dump16( (0xfffffff0&(int)str0)+0x30 );

        printf("duplicate string literals with optimization\n");
        printf("0x%08x 0x%08x 0x%08x 0x%08x\n",str0,str1,str2,str3);
        printf("0x%08x 0x%08x 0x%08x\n",str4,str5,str6);
    }

    pp_printf(dashline);
    if (1)
    {
        pp_thread_create(pp_stack_trace_test);
        pp_thread_delay(1000);
        pp_memory_dump16( (0xfffffff0&(int)pp_stack_trace_test_two_local2)-0x30 );
        pp_memory_dump16( (0xfffffff0&(int)pp_stack_trace_test_two_local2)-0x20 );
        pp_memory_dump16( (0xfffffff0&(int)pp_stack_trace_test_two_local2)-0x10 );
        pp_memory_dump16( (0xfffffff0&(int)pp_stack_trace_test_two_local2)+0x00 );
        pp_memory_dump16( (0xfffffff0&(int)pp_stack_trace_test_two_local2)+0x10 );
        pp_memory_dump16( (0xfffffff0&(int)pp_stack_trace_test_two_local2)+0x20 );
        pp_memory_dump16( (0xfffffff0&(int)pp_stack_trace_test_two_local2)+0x30 );
        printf("0x%08x 0x%08x\n",pp_stack_trace_test_one,pp_stack_trace_test_two);
    }

    pp_printf(dashline);
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

static void pp_nine_lines_edit(char **lines,int row,int column)
{
    int   rx;
    char *line;
    char  line128[128];
    char  ln;

    while (1)
    {
        for (rx=0; rx<row; rx++)
        {
            line = (char *)(((int)lines) + rx*column);
            line[column-1] = '\0';
            if (!isprint(line[0]))
                line[0] = '\0';

            pp_printf(" %d  %s\n",rx,line);
        }        

        pp_printf(" Selection : ");
        fgets(line128,sizeof(line128),stdin);
        ln = line128[0];
        if (ln == '\n' || ln == '\r')
            continue;        
        ln -= '0';
        if (ln < 0 || ln >= row)
            break;

        pp_printf(" %d  ",ln);

        fgets(line128,sizeof(line128),stdin);
        line = (char *)(((int)lines) + ln*column);
        strncpy(line,line128,column);

        pp_printf("\n");
    }
}

/*=========================================================== 
 * description:
 * parameters:
 * returns:
 */

static char matrix[9][64];

void pp_edit_line()
{
    pp_nine_lines_edit(matrix,9,64);    
}

/*================================================================ pp_env_show
 * description:
 * parameters:
 * returns:
 */

void pp_env_show()
{
    extern char **environ;
    int ix;

    ix = 0;
    while (environ[ix])
    {
         pp_printf("%s\n",environ[ix]);
         ix++;
    }
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

void pp_dev_mem_display(void *phy_adrs)
{
    size_t         len256 = 256;
    int            mfd;
    unsigned int   masked_adrs = ((unsigned int)phy_adrs & ~(len256-1));
    void          *mapped_adrs;
    unsigned char *disp_adrs;
    int            row, col;

    mfd = open("/dev/mem",O_RDONLY);
    if (mfd == -1)
    {
        perror(__FUNCTION__);
        printf("%s(%d)\n",__FILE__,__LINE__);
        return;
    }

    mapped_adrs = mmap(0x0,len256,PROT_READ,MAP_SHARED,mfd,(off_t)(masked_adrs));
    if (mapped_adrs == (void *)-1)
    {
        perror(__FUNCTION__);
        printf("%s(%d)\n",__FILE__,__LINE__);
        close(mfd);
        return;
    }

    printf("%08x <= %08x\n",masked_adrs,mapped_adrs);

    disp_adrs = (unsigned char *)mapped_adrs;
    for (row=0; row<(len256/16); row++)
    {
        printf("%08x: ", masked_adrs+((unsigned int)disp_adrs-(unsigned int)mapped_adrs));
        for (col=0; col<4; col++)
        {
            printf(" %08x",*(unsigned int *)disp_adrs);
            disp_adrs += 4;
        }

        printf(" ");

        disp_adrs -= 16;
        for (col=0; col<16; col++)
        {
            if (isprint(*disp_adrs))
            {
                printf("%c",*disp_adrs);
            }
            else
            {
                printf(".");
            }
            disp_adrs++;
        }

        printf("\n");
    }

    munmap(mapped_adrs,len256);

    close(mfd);
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

void pp_dev_mem_probe(void *phy_adrs,char mode,int length,char *value)
{
    int            open_flag;
    size_t         mmap_len;
    int            mmap_prot;
    off_t          mmap_off = (off_t)phy_adrs;
    int            mfd;
    void          *mapped_adrs;

    if (mode == 'r')
    {
        open_flag = O_RDONLY;
        mmap_prot = PROT_READ;
    }
    else if (mode == 'w')
    {
        open_flag = O_RDWR|O_SYNC;
        mmap_prot = PROT_READ|PROT_WRITE;
    }
    else
    {
        printf("%s(%d)\n",__FILE__,__LINE__);
        return;
    }

    if (length != 1 && length != 2 && length != 4 && length != 8)
    {
        printf("%s(%d)\n",__FILE__,__LINE__);
        return;
    }
    else
    {
        mmap_len = length;
    }

    mfd = open("/dev/mem",open_flag);
    if (mfd == -1)
    {
        perror(__FUNCTION__);
        printf("%s(%d)\n",__FILE__,__LINE__);
        return;
    }

    mapped_adrs = mmap(0x0,mmap_len,mmap_prot,MAP_SHARED,mfd,mmap_off);
    if (mapped_adrs == (void *)-1)
    {
        perror(__FUNCTION__);
        printf("%s(%d)\n",__FILE__,__LINE__);
        close(mfd);
        return;
    }

    if (mode == 'r')
    {
        memcpy(value,mapped_adrs,length);
    }
    else if (mode == 'w')
    {
        memcpy(mapped_adrs,value,length);
    }

    printf("%02x %04x %08x at %08x <= %08x\n",
        *(unsigned char  *)mapped_adrs,
        *(unsigned short *)mapped_adrs,
        *(unsigned int   *)mapped_adrs,
        (int)mmap_off,(int)mapped_adrs);

    munmap(mapped_adrs,mmap_len);

    close(mfd);
}

void pp_dev_mem_probe_test()
{
    char buf[16];

    pp_dev_mem_probe((void *)0x000a0000,'r',4,buf);

    buf[0] = 0xa5;
    buf[1] = 0xa6;
    buf[2] = 0xa7;
    buf[3] = 0xa8;
    pp_dev_mem_probe((void *)0x000a0000,'w',4,buf);

    pp_dev_mem_probe((void *)0x000a0000,'r',4,buf);
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

void pp_proc_iomem_display()
{
    FILE          *file;
    char           line[128];
    char          *ptrptr;
    char          *begin_adrs;
    char          *end_adrs;
    char          *device_name;
    unsigned int   device_adrs;

    file = fopen("/proc/iomem","r");
    if (file == NULL)
        return;

    while (fgets(line,sizeof(line),file) != NULL)
    {
        begin_adrs  = strtok_r(line,"-:\t\n",&ptrptr);
        end_adrs    = strtok_r(NULL,"-:\t\n",&ptrptr);
        device_name = strtok_r(NULL,"-:\t\n",&ptrptr);

        if (NULL != strstr(device_name,"reserved"))
        {
            continue;
        }

        device_adrs = strtoul ( begin_adrs, &ptrptr, 16 );

        if (device_adrs > 0x00100000)
        {
            break;
        }

        printf("%s-%s %s\n",begin_adrs,end_adrs,device_name);

        pp_dev_mem_display(device_adrs);
    }

    fclose(file);
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

void pp_time()
{
    clock_t     the_processor_time_since_the_program_started;
    time_t      the_current_calendar_time_of_the_system;
    time_t      current;
    struct tm * the_pointer_to_the_current_time;
    struct tm * the_pointer_to_the_current_gmt;
    int         dx;

    the_processor_time_since_the_program_started = clock();
    pp_printf("%d\n",the_processor_time_since_the_program_started);
    pp_thread_delay(1000);
    the_processor_time_since_the_program_started = clock();
    pp_printf("%d\n",the_processor_time_since_the_program_started);

    the_current_calendar_time_of_the_system = time(&current);
    pp_printf("current calendar time   %d %d\n",the_current_calendar_time_of_the_system,current);
    pp_thread_delay(1000);
    the_current_calendar_time_of_the_system = time(&current);
    the_pointer_to_the_current_time = localtime(&the_current_calendar_time_of_the_system);
    the_pointer_to_the_current_gmt = gmtime(&current);
    pp_printf("current calendar time   %d %d\n",the_current_calendar_time_of_the_system,current);
    pp_printf("ctime(current)          %s",ctime(&the_current_calendar_time_of_the_system));
    pp_printf("asctime(current)        %s",asctime(the_pointer_to_the_current_time));
    pp_printf("asctime(gmt)            %s",asctime(the_pointer_to_the_current_gmt));

    the_current_calendar_time_of_the_system = time(&current);
    pp_printf("current calendar time   %d %d\n",the_current_calendar_time_of_the_system,current);

    for (dx=0; dx<2000; dx++)
    {
        pp_thread_delay(1);
    }

    the_current_calendar_time_of_the_system = time(&current);
    pp_printf("current calendar time   %d %d\n",the_current_calendar_time_of_the_system,current);
}

/*================================================================= pp_up_time
 * description:
 * parameters:
 * returns:
 */

void pp_up_time()
{
}

/*========================================================= pp_shell_stub_init
 * description:
 * parameters:
 * returns:
 */

void pp_shell_stub_init()
{
}

