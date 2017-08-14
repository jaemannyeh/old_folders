/* pp_shared_tag.c - */

/************************************************************ include files */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "pp_base.h"

/****************************************************************** defines */

/******************************************************************** types */

typedef struct
{
    struct {
        unsigned int alloc      : 1;
        unsigned int reserved1  : 1;
        unsigned int reserved2  : 2;
        unsigned int type       : 8;  /* c:char s:short i:int f:float d:double */
        unsigned int reserved8  : 8;    
        unsigned int size       : 12;
    } header;

    char data[4];                     /* must be last */
} pp_shared_block_t;

typedef struct
{
    unsigned int base       : 8;      /* 0...15 */    
    unsigned int offset     : 24;
} pp_based_offset_t;

typedef struct
{
    pp_based_offset_t  based_offset;    /* must be first */

    struct {
        unsigned int alloc      : 1;
        unsigned int init       : 1;
        unsigned int reserved2  : 2;
        unsigned int type       : 8;  /* c:char s:short i:int f:float d:double */
        unsigned int reserved8  : 8;    
        unsigned int size       : 12;
    } header;    

    char ivalue[4];                   /* must be last */
} pp_shared_tag_t;

/************************************************ extern & local prototypes */

/********************************************************* extern & globals */

/******************************************************************* locals */

static const char *extension_tag_db  = ".tag.db";
static const char *extension_tag_txt = ".tag.txt";
static const char *extension_tag_off = ".tag.off";

#ifdef PP_SHARED_TAG_USE_BERKELEY_DB
    #include <db.h>         /* should be after #include "pp_base.h" */
    static DB *dbp = NULL;
#endif

static const int   shared_memory_segment_size = 1024 * 1024;
static void       *shared_memory_segment_start_address[16] = { NULL }; 
static void       *shared_memory_segment_end_address[16] = { NULL };

/*================================================= pp_shared_tag_get_filename
 * description:
 * parameters:
 * returns:
 */

static char *pp_shared_tag_get_filename(char *extension)
{
    static char   db_filename[128];
    static char   txt_filename[128];
    static char   off_filename[128];
    char         *filename;

    if(strcmp(extension,extension_tag_db) == 0)
        filename = db_filename;
    else if(strcmp(extension,extension_tag_txt) == 0)
        filename = txt_filename;
    else if(strcmp(extension,extension_tag_off) == 0)
        filename = off_filename;
    else
        return NULL;
    
    strcpy( filename, getenv("_") );
#if defined(PP_CYGWIN)
    strcpy( strstr(filename,".exe"), extension );
#else
    strcat( filename, extension );
#endif

    return filename;
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

static void pp_shared_block_shm_attach(int base)
{
#if defined(PP_LINUX)
    key_t key;
    int   shmid;

#ifdef PP_SHARED_TAG_USE_BERKELEY_DB
    key = ftok(pp_shared_tag_get_filename(extension_tag_db), base);
#else
    key = ftok(pp_shared_tag_get_filename(extension_tag_txt), base);
#endif
    if (key == (key_t)-1)
    {
        pp_printf("%s(%d)\n",__FUNCTION__,__LINE__);
        perror(__FUNCTION__);
        return;
    }

    shmid = shmget(key, shared_memory_segment_size, 0666|IPC_CREAT);

    shared_memory_segment_start_address[base] = shmat(shmid, (void *)0, 0);
#else
    shared_memory_segment_start_address[base] = malloc(shared_memory_segment_size);
#endif

    shared_memory_segment_end_address[base] = shared_memory_segment_start_address[base] + shared_memory_segment_size - 1;
    pp_printf("%x = %x + %x - 1\n",shared_memory_segment_end_address[base],
                            shared_memory_segment_start_address[base],
                            shared_memory_segment_size);

    memset(shared_memory_segment_start_address[base],0,shared_memory_segment_size);
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

static pp_shared_block_t *pp_shared_block_alloc_with_base(int base,int size)
{
    pp_shared_block_t *block;
    int              aligned_size;

    if (size <= 0)
    {
        pp_printf("%s(%d)\n",__FUNCTION__,__LINE__);
        return NULL;
    }

    if (shared_memory_segment_start_address[base] == NULL)
    {
        pp_shared_block_shm_attach(base);
    }

    block = shared_memory_segment_start_address[base];
    if (block == NULL)
    {
        pp_printf("%s(%d)\n",__FUNCTION__,__LINE__);
        return NULL;
    }

    aligned_size = ((size + 3) / 4) * 4;

    while (1)
    {
        int data_size;
        int aligned_data_size;

        if (block > shared_memory_segment_end_address[base])
        {
            pp_printf("%s(%d)\n",__FUNCTION__,__LINE__);
            return NULL;
        }

        if (block->header.size == 0)
            break;

        data_size = block->header.size;
        aligned_data_size = ((data_size + 3) / 4) * 4;
        if ((aligned_size == aligned_data_size) && block->header.alloc == 0)
            break;

        block = ((char *)block) + 4;
        block = ((char *)block) + aligned_data_size;
    }

    if (block + aligned_size > shared_memory_segment_end_address[base])
    {
        pp_printf("%s(%d)\n",__FUNCTION__,__LINE__);
        return NULL;
    }

    block->header.alloc = 1;
    block->header.size = size;

    return block;
/*
    offset = ((int)&(block->data[0])) - ((int)shared_memory_segment_start_address[base]);

    based_offset = (base << 24) | offset;

    return based_offset;
*/
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

static pp_shared_block_t *pp_shared_block_alloc(int size)
{
    int              base;
    pp_shared_block_t *block;

    for (base=0; base<16; base++)
    {
        block = pp_shared_block_alloc_with_base(base,size);
        if (block != NULL)
            break;
    }    

    return block;
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

static unsigned int pp_shared_block_get_base_from_block(pp_shared_block_t *block)
{
    unsigned int base;

    for (base=0; base<16; base++)
    {
        if (block <= shared_memory_segment_end_address[base] &&
            block >= shared_memory_segment_start_address[base])
            return base;
    }

    pp_printf("%s(%d)",__FUNCTION__,__LINE__);
    exit(0);
    return -1;
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

static unsigned int pp_shared_block_get_offset_from_block(pp_shared_block_t *block)
{
    unsigned int base;
    unsigned int offset;

    base = pp_shared_block_get_base_from_block(block);

    offset = (unsigned int)block - (unsigned int)shared_memory_segment_start_address[base];

    return offset;
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

static void pp_shared_block_load_init_value(pp_shared_block_t *block,pp_shared_tag_t *tag)
{
    char      type;
    int       number_of_data;
    int       index;

    char     *data_1;
    short    *data_2;
    int      *data_4;
    float    *data_f;
    char     *data_s;

    char     *ivalue;
    char     *ptrptr;
    char     *token;
    char     *endptr;

    if (tag->header.init == 0)
    {
        return;    
    }

    type = tag->header.type;

    if (type == '1')
        number_of_data = tag->header.size / 1;
    else if (type == '2')
        number_of_data = tag->header.size / 2;
    else if (type == '4')
        number_of_data = tag->header.size / 4;
    else if (type == 'f')
        number_of_data = tag->header.size / 4;
    else if (type == 's')
        number_of_data = tag->header.size / 1;
    else
        return;

    data_1 = data_2 = data_4 = data_f = data_s = &(block->data[0]);

    ivalue = strdup(tag->ivalue);

    token = strtok_r(ivalue,", \t",&ptrptr);
    
    index = 0;

    do {
        if (token == NULL)
            break;

        if (type == '1')
        {
            if (token[0] == '0' && token[1] == 'x')
                data_1[index++] = (char)strtoul(token,&endptr,16);
            else
                data_1[index++] = (char)atoi(token);
        }
        else if (type == '2')
        {
            if (token[0] == '0' && token[1] == 'x')
                data_2[index++] = (short)strtoul(token,&endptr,16);
            else
                data_2[index++] = (short)atoi(token);
        }
        else if (type == '4')
        {
            if (token[0] == '0' && token[1] == 'x')
                data_4[index++] = (int)strtoul(token,&endptr,16);
            else
                data_4[index++] = (int)atoi(token);
        }
        else if (type == 'f')
        {
            data_f[index++] = (float)atof(token);
        }
        else if (type == 's')
        {
            strcpy(data_s,ivalue);
            break;
        }
        else
        {
            break;
        }

        token = strtok_r(NULL,", \t",&ptrptr);      
    } while (index < number_of_data);

    free(ivalue);

    while (index < number_of_data)
    {
        if (type == '1')
            data_1[index] = data_1[index-1];
        else if (type == '2')
            data_2[index] = data_2[index-1];
        else if (type == '4')
            data_4[index] = data_4[index-1];
        else if (type == 'f')
            data_f[index] = data_f[index-1];
        else if (type == 's')
                ;
        else
                ;

        index++;
    }
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

static pp_shared_block_t *pp_shared_block_alloc_with_tag(pp_shared_tag_t *tag)
{
    pp_shared_block_t *block;

    block = pp_shared_block_alloc(tag->header.size);
    
    block->header.type = tag->header.type;

    pp_shared_block_load_init_value(block,tag);

    tag->based_offset.base   = pp_shared_block_get_base_from_block(block);
    tag->based_offset.offset = pp_shared_block_get_offset_from_block(block);

    return block;
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

static void pp_shared_block_free(pp_shared_block_t *block)
{
    if (block->header.alloc == 0)
        return;

    if (block->header.size == 0)
        return;

    block->header.alloc = 0;
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

void pp_shared_block_show_with_base(int base)
{
    pp_shared_block_t *block;

    if (shared_memory_segment_start_address[base] == NULL)
        return;

    block = (pp_shared_block_t *)shared_memory_segment_start_address[base];

    while (1)
    {
        int data_size;
        int aligned_data_size;

        if (block->header.size == 0)
            break;

        data_size = block->header.size;
        aligned_data_size = ((data_size + 3) / 4) * 4;

        pp_printf("%08x %08x %d %c %6d ",block,*(int *)block,block->header.alloc,block->header.type,block->header.size);

        if (block->header.type == '1')
        {
            pp_printf("%d",*(char *)block->data);
        }
        else if (block->header.type == '2')
        {
            pp_printf("%d",*(short *)block->data);
        }
        else if (block->header.type == '4')
        {
            pp_printf("%d",*(int *)block->data);
        }
        else if (block->header.type == 'f')
        {
            pp_printf("%f",*(float *)block->data);
        }
        else if (block->header.type == 's')
        {
            pp_printf("%s",(char *)block->data);
        }
        else
        {
        }

        pp_printf("\n");

        block = ((char *)block) + 4;
        block = ((char *)block) + aligned_data_size;
    }
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

void pp_shared_block_show()
{
    int bx;

    pp_printf("address  header   a t      s ivalue\n");
    for (bx=0; bx<16; bx++)
    {
        pp_shared_block_show_with_base(bx);
    }    
}

/*======================================================== pp_shared_tag_alloc
 * description:
 * parameters:
 * returns:
 */

pp_based_offset_t pp_shared_tag_alloc(char *tag_name,int tag_type,int number_of_data,char *ivalue)
{
    pp_shared_tag_t    *tag; 
    int               tag_size;
    int               ivalue_length;
    pp_shared_block_t  *block; 
    pp_based_offset_t   based_offset = { -1, -1 };
    
    if (tag_name == NULL || tag_name[0] == '\0' || !isprint(tag_name[0]))
    {
        pp_printf("%s(%d)\n",__FUNCTION__,__LINE__);
        return based_offset;
    }

    if (tag_type == '1')
    {
        tag_size = 1 * number_of_data;
    }
    else if (tag_type == '2')
    {
        tag_size = 2 * number_of_data;
    }
    else if (tag_type == '4')
    {
        tag_size = 4 * number_of_data;
    }
    else if (tag_type == 'f')
    {
        tag_size = 4 * number_of_data;
    }
    else if (tag_type == 's')
    {
        tag_size = strlen(ivalue)+1;
    }
    else
    {
        pp_printf("%s(%d)\n",__FUNCTION__,__LINE__);
        return based_offset;
    }

    if (number_of_data <= 0)
    {
        pp_printf("%s(%d)\n",__FUNCTION__,__LINE__);
        return based_offset;
    }

    if (ivalue == NULL || ivalue[0] == '\0' || !isprint(ivalue[0]))
    {
        ivalue_length = 0;
    }
    else
    {
        ivalue_length = strlen(ivalue);
    }

    tag = malloc(sizeof(pp_shared_tag_t)-4+ivalue_length+4);

    tag->header.type = tag_type;

    tag->header.size = tag_size;

    if (ivalue_length == 0)
    {
        tag->header.init = 0;
        tag->ivalue[0] = '\0';
    }
    else
    {
        tag->header.init = 1;    
        strcpy(tag->ivalue,ivalue);
    }

    block = pp_shared_block_alloc_with_tag(tag);
    based_offset = tag->based_offset;

#ifdef PP_SHARED_TAG_USE_BERKELEY_DB
    if (1)
    {
        DBT              key;
        DBT              data;
        int              rx;

        pp_shared_tag_db_open();

        memset(&key,0,sizeof(key));
        key.data = tag_name;
        key.size = strlen(tag_name);

        memset(&data,0,sizeof(data));
        data.data = tag;
        data.size = sizeof(pp_shared_tag)-4+ivalue_length;

        rx = dbp->put(dbp,NULL,&key,&data,0);
        if (rx != 0)
        {
            dbp->err(dbp,rx,"dbp->put");
        }

        pp_shared_tag_db_close();
    }
#endif

    free(tag);

    return based_offset;
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

static void pp_shared_tag_db_open()
{
#ifdef PP_SHARED_TAG_USE_BERKELEY_DB
    /* refer to http://pybsddb.sourceforge.net/ref/simple_tut */
    int rx;

    if (dbp != NULL)
    {
        pp_printf("%s(%d)\n",__FUNCTION__,__LINE__);
        return;
    }

    rx = db_create(&dbp,NULL,0);
    if (rx != 0 || dbp == NULL)
    {
        pp_printf("%s(%d) %s\n",__FUNCTION__,__LINE__,db_strerror(rx));
        return;
    }

    rx = dbp->open(dbp,NULL,pp_shared_tag_get_filename(extension_tag_db),NULL,DB_BTREE,DB_CREATE,0664);
    if (rx != 0)
    {
        pp_printf("%s(%d)\n",__FUNCTION__,__LINE__);
        dbp->err(dbp,rx,"%s",pp_shared_tag_get_filename(extension_tag_db));
        return;
    }
#endif
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

static void pp_shared_tag_db_close()
{
#ifdef PP_SHARED_TAG_USE_BERKELEY_DB
    int rx;

    if (dbp == NULL)
    {
        pp_printf("%s(%d)\n",__FUNCTION__,__LINE__);
        return;    
    }

    rx = dbp->close(dbp,0);
    if (rx != 0)
    {
        pp_printf("%s(%d) %s\n",__FUNCTION__,__LINE__,db_strerror(rx));
        return;
    }

    dbp = NULL;
#endif
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

void pp_shared_tag_db_del(char *tag_name)
{
#ifdef PP_SHARED_TAG_USE_BERKELEY_DB
    DBT  key;
    int  rx;

    pp_shared_tag_db_open();

    memset(&key,0,sizeof(key));
    key.data = tag_name;
    key.size = strlen(tag_name);

    rx = dbp->del(dbp,NULL,&key,0);
    if (rx != 0)
    {
        pp_printf("%s(%d)\n",__FUNCTION__,__LINE__);
        dbp->err(dbp,rx,"dbp->del");
    }

    pp_shared_tag_db_close();
#endif
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

void pp_shared_tag_get(char *tag_name)
{
#ifdef PP_SHARED_TAG_USE_BERKELEY_DB
    pp_shared_tag_t *tag; 
    DBT            key;
    DBT            data;
    int            rx;

    pp_shared_tag_db_open();

    memset(&key,0,sizeof(key));
    key.data = tag_name;
    key.size = strlen(tag_name);

    memset(&data,0,sizeof(data));

    rx = dbp->get(dbp,NULL,&key,&data,0);
    if (rx != 0)
    {
        pp_printf("%s(%d)\n",__FUNCTION__,__LINE__);
        dbp->err(dbp,rx,"dbp->get");
    }
    else
    {
        tag = data.data;
        pp_printf("%.*s  %d %d %d\n", (int)key.size,(char *)key.data,
                                   tag->header.init,tag->header.type,tag->header.size);

    }

    pp_shared_tag_db_close();
#endif
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

void pp_shared_tag_show()
{
#ifdef PP_SHARED_TAG_USE_BERKELEY_DB
    pp_shared_tag_t *tag; 
    DBC           *dbcp;
    DBT            key;
    DBT            data;
    int            rx;

    pp_shared_tag_db_open();

    dbp->cursor(dbp,NULL,&dbcp,0);

    memset(&key,0,sizeof(key));
    memset(&data,0,sizeof(data));

    pp_printf("name                s  b        o i t        s i\n");
    while (1)
    {
        rx = dbcp->c_get(dbcp,&key,&data,DB_NEXT);    
        if (rx != 0)
        {
            break;
        } 
        else
        {
            char name[16];

            memset(name,' ',sizeof(name));
            strncpy(name,key.data,PP_MIN(sizeof(name),key.size));

            tag = data.data;

            pp_printf("%.*s %4d %2d %8d %1d %c %8d ",sizeof(name),name,
                                          data.size,
                                          tag->based_offset.base,
                                          tag->based_offset.offset,
                                          tag->header.init,
                                          tag->header.type,
                                          tag->header.size);
            if (tag->header.init == 1)
                pp_printf("%.*s ",data.size-8,tag->ivalue);
            pp_printf("\n");
        }
    }

    dbcp->c_close(dbcp);

    pp_shared_tag_db_close();
#endif
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

void pp_shared_tag_test()
{
#if 0
    pp_shared_tag_alloc ( "tag31", '1', 13, NULL ); 
    pp_shared_tag_alloc ( "tag37", '1', 17, "0x0f" ); 
    pp_shared_tag_alloc ( "tag41", 's', 1, "string" ); 

    pp_shared_tag_alloc ( "tag02", '1', 1, NULL ); 
    pp_shared_tag_alloc ( "tag03", '2', 1, NULL ); 
    pp_shared_tag_alloc ( "tag05", '4', 1, "2,3,5,7,11,13,17,19" ); 
    pp_shared_tag_alloc ( "tag07", 'f', 1, "1.61803399" ); 
    pp_shared_tag_alloc ( "tag11", 'f', 1, "3.14159265" ); 

    pp_shared_tag_alloc ( "tag13", '1', 2, "-1" ); 
    pp_shared_tag_alloc ( "tag17", '2', 3, "-1234" ); 
    pp_shared_tag_alloc ( "tag19", '4', 5, "2,3,5,7,11,13,17,19" ); 
    pp_shared_tag_alloc ( "tag23", 'f', 7, "1.234" ); 
    pp_shared_tag_alloc ( "tag29", 'f', 11, "1.23456789,2.123456789" ); 
#endif
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

void pp_shared_tag_load_txt()
{
    FILE            *file_tag_txt;
    FILE            *file_tag_off;
    char             line[128];

    char            *px;
    char            *ptrptr;
    char            *tag_name;
    char            *tag_type;
    char            *tag_number_of_data;
    char            *tag_ivalue;

    pp_based_offset_t  based_offset;

    file_tag_txt = fopen(pp_shared_tag_get_filename(extension_tag_txt),"r");
    if (file_tag_txt == NULL)
    {
        pp_printf("%s(%d)\n",__FUNCTION__,__LINE__);
        return;
    }
    file_tag_off = fopen(pp_shared_tag_get_filename(extension_tag_off),"w");
    if (file_tag_off == NULL)
    {
        close(file_tag_txt);
        pp_printf("%s(%d)\n",__FUNCTION__,__LINE__);
        return;
    }

    while (fgets(line,sizeof(line),file_tag_txt) != NULL)
    {
        px = strchr( line, '#' );
        if (px != NULL)
            *px = '\0';

        tag_name = strtok_r(line,":",&ptrptr);
        if (tag_name == NULL || !isalpha(tag_name[0]))
            continue;

        tag_type = strtok_r(NULL,":",&ptrptr);      
        if (tag_type == NULL)
            continue;

        tag_number_of_data = strtok_r(NULL,":",&ptrptr);      

        tag_ivalue = strtok_r(NULL,":",&ptrptr);      
        px = strchr( tag_ivalue, 0x0a );
        if (px != NULL)
            *px = '\0';

        /* pp_printf("%s %s %s %s\n",tag_name,tag_type,tag_number_of_data,tag_ivalue); */

        based_offset =  pp_shared_tag_alloc(tag_name,tag_type[0],atoi(tag_number_of_data),tag_ivalue);
        fprintf(file_tag_off,"%s:0x%x:0x%08x\n",tag_name,based_offset.base,based_offset.offset);
    }

    fclose(file_tag_off);
    fclose(file_tag_txt);
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

void pp_shared_tag_load_db()
{
}

/*===========================================================
 * description:
 * parameters:
 * returns:
 */

void pp_shared_tag_init()
{
}

