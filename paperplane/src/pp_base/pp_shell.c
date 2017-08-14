/* pp_shell.c - */

/************************************************************ include files */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>        /* offsetof() */
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>

#include "pp_base.h"

/****************************************************************** defines */

/******************************************************************** types */

typedef struct
{
    char        *name;
    void        (*func)();    
} pp_command_t;

/************************************************ extern & local prototypes */

static int pp_shell_func_get(char *name);

/********************************************************* extern & globals */

/******************************************************************* locals */

#ifdef PP_LOG
    static int pp_shell_log_level = PP_LOG_DEBUG;
    #undef  PP_LOG_LEVEL
    #define PP_LOG_LEVEL(x)    (pp_shell_log_level >= x)
#else
    #define PP_LOG             do {} while(0)    
#endif

static char            pp_shell_argv[8][32];
static int             pp_shell_argc;
static pthread_mutex_t pp_shell_mutex = PTHREAD_MUTEX_INITIALIZER;

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

static void bye()
{
    exit(1);    
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

static void convert(char *arg1)
{
    char        *endptr;
    int          hex;
    int          ix;

    hex = strtoul ( arg1, &endptr, 16 );    /* hex = strtol ( arg1, &endptr, 16 ); */

    pp_printf("            ");
    for (ix=31; ix>=0; ix--)
    {
        pp_printf("%02d ",ix);
    }
    pp_printf("\n");

    pp_printf("            ");
    for (ix=0; ix<32; ix++)
    {
        pp_printf("%02d ",ix);
    }
    pp_printf("\n");

    pp_printf("            ");
    for (ix=32; ix<64; ix++)
    {
        pp_printf("%02d ",ix);
    }
    pp_printf("\n");

    pp_printf("0x%08x  ",hex);

    for (ix=31; ix>=0; ix--)
    {
        pp_printf(" %d ", (hex & (1 << ix)) ? 1 : 0 );
    }

    pp_printf("\n");
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

static void memory_display_physical(char *arg1)
{
    char                 *endptr;
    static unsigned int   addr = 0; /* static to keep its progress */
    int                   ix;

    if (arg1 != NULL && arg1[0] != 0x00)
        addr = strtoul ( arg1, &endptr, 16 );    /* addr = strtol ( arg1, &endptr, 16 ); */

    if (addr == 0)
        return;    

    pp_dev_mem_display( addr );
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

static void memory_display_virtual(char *arg1)
{
    char                 *endptr;
    static unsigned int   addr = 0; /* static to keep its progress */
    int                   ix;

    if (arg1 != NULL && arg1[0] != 0x00)
        addr = strtoul ( arg1, &endptr, 16 );    /* addr = strtol ( arg1, &endptr, 16 ); */

    if (addr == 0)
        return;    

    pp_memory_dump16( addr );

#if 1
    addr &= 0xfffffff0;
    addr += 16;

    for (ix=0; ix<16; ix++)
    {
        pp_memory_dump16( addr );
        addr += 16;
    }
#endif
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

static void repeat(char *arg1)
{
    int     nx;
    void  (*func)();    
    int     ix;

    nx = atoi(arg1);
    if (nx == 0)
        return;

    func = pp_shell_func_get(pp_shell_argv[2]);
    if (func == NULL)
        return;	    

    for (ix=0; ix<nx; ix++)
    {
        (func)(NULL);
    }
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

static void version()
{
    pp_printf("%s %s\n",__DATE__,__TIME__);    
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

static pp_command_t pp_internal_commands[] = {
    { "bye",         bye },    
    { "convert",     convert },    
    { "mdp",          memory_display_physical },    
    { "mdv",          memory_display_virtual },    
    { "repeat",      repeat },
    { "version",     version },    
    { NULL,          NULL }    
};

static void help()
{
    pp_command_t *command;

    command = &pp_internal_commands[0];
    while (command->name != NULL)
    {
        pp_printf("%s\n",command->name);
        command++;
    }
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

static int pp_shell_func_get(char *name)
{
    pp_command_t  *command;
    void       (*func)() = NULL;

    command = &pp_internal_commands[0];
    while (command->name != NULL && func == NULL)
    {
        if (strcmp(command->name,name) == 0)
        {
            func = command->func;
        }
        command++;      
    }

    if (func == NULL)
    {
        func = pp_name_get(name);
    }

    return func;	
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

static char pp_shell_line[128];

static void pp_shell_execute()
{
    void (*func)();

    if (pp_shell_argv[0][0] == 0x00 || 
        pp_shell_argv[0][0] == 0x0d || 
        pp_shell_argv[0][0] == 0x0a)
        return;

    /********************************/

    if (pp_shell_argv[0][0] == '0' && pp_shell_argv[0][1] == 'x')
    {
        char *endptr;
        int   value;

        value = strtol ( pp_shell_argv[0], &endptr, 16 );
        pp_printf("value = %d = 0x%x\n",value,value);    
        return;    
    }
    else if ('0' <= pp_shell_argv[0][0] && pp_shell_argv[0][0] <= '9')
    {
        pp_printf("value = %d = 0x%x\n",atoi(pp_shell_argv[0]),atoi(pp_shell_argv[0]));    
        return;    
    }

    /********************************/

    func = pp_shell_func_get(pp_shell_argv[0]);
    if (func == NULL)
    {
        pp_printf("%s: command not found\n",pp_shell_argv[0]);
        return;
    }

   (func)(pp_shell_argv[1],pp_shell_argv[2]);
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

static void pp_shell_parse()
{
    char *ptrptr;
    char *token;

    memset( pp_shell_argv, 0, sizeof(pp_shell_argv) );
    pp_shell_argc = 0;

    token = strtok_r(pp_shell_line," \t\n",&ptrptr);

    do {
        if (token == NULL)
            break;
        if (*token == '#')
            break;

        strncpy( pp_shell_argv[pp_shell_argc], token, 32 );
        pp_shell_argc++;

        token = strtok_r(NULL," \t\n",&ptrptr);      
    } while (pp_shell_argc < 8);
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

void pp_shell(char *line)
{
    char *px;

    if (line == NULL)
        return;

    px = strchr( line, 0x0d );
    if (px != NULL)
        *px = NULL;	    
    px = strchr( line, 0x0a );
    if (px != NULL)
        *px = NULL;	    

    pthread_mutex_lock( &pp_shell_mutex );

    strncpy( pp_shell_line, line, sizeof(pp_shell_line) );

    pp_shell_parse();

    pp_shell_execute();

    pthread_mutex_unlock( &pp_shell_mutex );
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

static char *pp_shell_prompt = "] ";

static void *pp_shell_loop(void *ptr)
{
    char line[128];

    pp_name_put("help",help);
    pp_name_put("pp_shell_prompt",pp_shell_prompt);

    while (1)
    {
        pp_printf(pp_shell_prompt);

        memset(line,0,sizeof(line));

        fgets(line,sizeof(line),stdin);

        pp_shell(line);
    }

    return NULL;
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

static char * pp_shell_get_init_filename()
{
    static char  init_filename[64];
    const char  *extension_init = ".init";

#if defined(PP_CYGWIN)
    strcpy( init_filename, getenv("_") );
    strcpy( strstr(init_filename,".exe"), extension_init );
#else
    strcpy( init_filename, getenv("_") );
    strcat( init_filename, extension_init );
#endif

    return init_filename;
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

void pp_shell_init()
{
    pp_shell_stub_init();

    PP_LOG(TRACE,"%s(%d)",__FUNCTION__,__LINE__,3,4,5,6);

    pp_name_put("pp_shell_log_level",&pp_shell_log_level);

    pp_thread_create(pp_shell_loop);
}

/*=========================================================== pp_shell_startup
 * description:
 * parameters:
 * returns:
 */

void pp_shell_startup()
{
    FILE *file;
    char  line[128];

    file = fopen(pp_shell_get_init_filename(),"r");
    if (file == NULL)
    {
        pp_printf("%s(%d)\n",__FUNCTION__,__LINE__);
        return;	    
    }

    while (fgets(line,sizeof(line),file) != NULL)
    {
        pp_shell(line);
    }

    fclose(file);
}

