/* pp_base.c - */

/************************************************************ include files */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
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

typedef struct
{
    int   into;
    int   from;
    int   size;
    char *buff;    
} pp_ring_t;

typedef struct pp_slist_node_s
{
    struct pp_slist_node_s *next;
} pp_slist_node_t;

typedef struct
{
    pp_slist_node_t *head;
    pp_slist_node_t *tail;
} pp_slist_t;

typedef struct
{
    pp_slist_node_t  node;
    int            length;
} pp_msg_node_t;

typedef struct
{
    int      number;
    int      length;
    pp_slist_t queued_list;
    pp_slist_t unqueued_list;
} pp_msg_que_t;

typedef struct pp_ht_entry_s
{
    void                      *k;
    void                      *v;
    unsigned int               h;
    struct pp_ht_entry_s *next;
} pp_ht_entry_t;

typedef struct
{
    unsigned int            tablelength;
    pp_ht_entry_t           **table;
    unsigned int            entrycount;
    unsigned int            loadlimit;
    unsigned int            primeindex;
    unsigned int          (*hashfn) (void *k);
    int                   (*eqfn) (void *k1, void *k2);
} pp_hashtable_t;

/************************************************ extern & local prototypes */

/********************************************************* extern & globals */

int pp_log_ring_size = 64 * 128;

/******************************************************************* locals */

static const unsigned int pp_ht_primes[] = {
           53,         97,       193,       389,
          769,       1543,      3079,      6151,
        12289,      24593,     49157,     98317,
       196613,     393241,    786433,   1572869,
      3145739,    6291469,  12582917,  25165843,
     50331653,  100663319, 201326611, 402653189,
    805306457, 1610612741
};

static const unsigned int pp_ht_prime_table_length = sizeof(pp_ht_primes)/sizeof(pp_ht_primes[0]);
static const float        pp_ht_max_load_factor = 0.65;

static pp_hashtable_t      *pp_names = NULL;

static void              *pp_log_ring = NULL;
static void             (*pp_log_hook)(char *message) = NULL;

/*================================================================== pp_printf
 * description:
 * parameters:
 * returns:
 */

static int              (*pp_write)(char *buf,size_t n) = NULL;

void pp_printf_set_write( int (*func)(char *buf,size_t n) )
{
    pp_write = func;
}

int pp_printf(const char *format,...)
{
    va_list ap;
    char    buf[1024];
    int     len;

    va_start(ap,format);

    len = vsnprintf(buf,sizeof(buf),format,ap);

    va_end(ap);

    if (pp_write == NULL)
        write(1,buf,len);
    else
        pp_write(buf,len);

    return len;
}

/*=========================================================== pp_memory_dump16
 * description:
 * parameters:
 * returns:
 */

void pp_memory_dump16(unsigned short * adrs)
{
    int             ix;
    unsigned char * byte;
    char            ascii[16+1];

    if (0x00000000 != (0x0000000f&(unsigned int)adrs))
    {
        /*    
        char space[64];
        char offset[16]={0,2,5,7,10,12,15,17,20,22,25,27,30,32,35,37};  

        memset(space,' ',sizeof(space));
        space[14+offset[0x0000000f&(unsigned int)adrs]] = '!';
        space[15+offset[0x0000000f&(unsigned int)adrs]] = '\0';
        pp_printf("%s\n",space);
        */
    }

    byte = (unsigned char *)(~0x0000000f & (unsigned int) adrs);
    pp_printf("0x%08x:  ", ~0x0000000f & (unsigned int) adrs);
    
    for (ix=0; ix<16; ix++)
    {
        if(isprint(*byte))
        {
            ascii[ix] = (char)*byte;
        }
        else
        {
            ascii[ix] = '.';
        }

        if ((ix & 0x00000001) == 0x00000000)
        {
            if ((unsigned int)byte < (unsigned int)adrs)
            {
                pp_printf("     ");
            }
            else
            {
#if 0
                pp_printf("%04x ",*(unsigned short *)byte); /* for big endian */
#else
                pp_printf("%02x",*byte);
                pp_printf("%02x ",*(byte+1));
#endif
            }
        }

        byte++;
    }
    ascii[16] = '\0';

    pp_printf("*%16s*\n",ascii);
}

/*============================================================== pp_ring_clear
 * description:
 * parameters:
 * returns:
 */

void pp_ring_clear(pp_ring_t *ring)
{
    ring->into = 0;
    ring->from = 0;
}

/*=========================================================== pp_memory_dump16
 * description:
 * parameters:
 * returns:
 */

pp_ring_t * pp_ring_create(int size)
{
    pp_ring_t *ring;

    ring = malloc(sizeof(pp_ring_t));

    ring->size = size;
    ring->buff = malloc(size+1); /*   +1 for boundary */
    ring->buff[size] = '\0';     /* null for boundary */

    pp_ring_clear(ring);

    return ring;
}

/*============================================================= pp_ring_delete
 * description:
 * parameters:
 * returns:
 */

void pp_ring_delete(pp_ring_t *ring)
{
    free(ring->buff);
    free(ring);
}

/*========================================================== pp_ring_duplicate
 * description:
 * parameters:
 * returns:
 */

pp_ring_t * pp_ring_duplicate(pp_ring_t *ring)
{
    pp_ring_t *r;

    r = pp_ring_create(ring->size);

    r->into = ring->into;
    r->from = ring->from;

    memcpy(r->buff,ring->buff,ring->size);

    return r;
}

/*========================================================= pp_ring_frpp_bytes
 * description:
 * parameters:
 * returns:
 */

int pp_ring_frpp_bytes(pp_ring_t *ring)
{
    int n = ring->from - ring->into - 1;

    if (n < 0)
        n += ring->size;

    return n;    
}

/*======================================================== pp_ring_get_advance
 * description:
 * parameters:
 * returns:
 */

void pp_ring_get_advance(pp_ring_t *ring,int n)
{
    n += ring->into;

    if (n >= ring->size)
        n -= ring->size;

    ring->into = n;    
}

/*================================================================ pp_ring_get
 * description:
 * parameters:
 * returns:
 */

int pp_ring_get(pp_ring_t *ring,char *buffer,int size)
{
    int into = ring->into;
    int bytesgot;
    int from2;
    int bytes2;

    if (into >= ring->from)
    {
        bytesgot = PP_MIN(size, into-ring->from);    
        memcpy(buffer, &(ring->buff[ring->from]), bytesgot); 
        ring->from += bytesgot;
    }
    else
    {
        bytesgot = PP_MIN(size, ring->size-ring->from);   
        memcpy(buffer, &(ring->buff[ring->from]), bytesgot); 
        from2 = ring->from + bytesgot;

        if (from2 == ring->size)
        {
            bytes2 = PP_MIN(size-bytesgot, into);    
            memcpy(buffer+bytesgot, &(ring->buff[0]), bytes2); 
            ring->from = bytes2;    
            bytesgot += bytes2;
        }
        else
        {
            ring->from = from2;
        }
    }

    return bytesgot;
}

/*========================================================= pp_ring_get_string
 * description:
 * parameters:
 * returns:
 */

int pp_ring_get_string(pp_ring_t *ring,char *buffer,int size)
{
    int bytesgot;

    bytesgot = PP_MIN( size, strlen( &(ring->buff[ring->from]) ) + 1 );

    return pp_ring_get(ring,buffer,bytesgot);
}

/*================================================================ pp_ring_put
 * description:
 * parameters:
 * returns:
 */

int pp_ring_put(pp_ring_t *ring,char *buffer,int size)
{
    int from = ring->from;
    int bytesput;
    int into2;
    int bytes2;

    if (from > ring->into)
    {
        bytesput = PP_MIN(size, from-ring->into-1);
        memcpy(&(ring->buff[ring->into]), buffer, bytesput);
        ring->into += bytesput;
    }
    else if (from == 0)
    {
        bytesput = PP_MIN(size, ring->size-ring->into-1);
        memcpy(&(ring->buff[ring->into]), buffer, bytesput);
        ring->into += bytesput;
    }
    else
    {
        bytesput = PP_MIN(size, ring->size-ring->into);
        memcpy(&(ring->buff[ring->into]), buffer, bytesput);
        into2 = ring->into + bytesput;

        if (into2 == ring->size)
        {
            bytes2 = PP_MIN(size-bytesput,from-1);
            memcpy(&(ring->buff[0]), buffer+bytesput, bytes2);
            ring->into = bytes2;
            bytesput += bytes2;
        }
        else
        {
            ring->into = into2;
        }
    }

    return bytesput;
}

/*===================================================== pp_ring_put_with_shift
 * description:
 * parameters:
 * returns:
 */

int pp_ring_put_with_shift(pp_ring_t *ring,char *buffer,int size)
{
    if (size > pp_ring_frpp_bytes(ring))
    {
        pp_ring_get_advance(ring,size);
    }

    return pp_ring_put(ring,buffer,size);
}

/*============================================================== pp_slist_init
 * description:
 * parameters:
 * returns:
 */


void pp_slist_init(pp_slist_t *list)
{
    list->head = NULL;
    list->tail = NULL;   
}

/*=============================================================== pp_slist_put
 * description:
 * parameters:
 * returns:
 */

void pp_slist_put(pp_slist_t *list,pp_slist_node_t *node)
{
    node->next = NULL;

    if (list->head == NULL)
    {
        list->head = node;
        list->tail = node;
    }
    else
    {
        list->tail->next = node;
        list->tail = node;
    }
}

/*=============================================================== pp_slist_get
 * description:
 * parameters:
 * returns:
 */

pp_slist_node_t *pp_slist_get(pp_slist_t *list)
{
    pp_slist_node_t *node;

    node = list->head;
    if (node != NULL)
    {
        list->head = node->next;

        if (list->tail == node)
            list->tail = NULL;
    }
    return node;
}

/*========================================================== pp_slist_is_empty
 * description:
 * parameters:
 * returns:
 */

int pp_slist_is_empty(pp_slist_t *list)
{
    return (list->head == NULL);
}

/*================================================================= pp_mq_open
 * description:
 * parameters:
 * returns:
 */

pp_msg_que_t *pp_mq_open(int number,int length)
{
    pp_msg_que_t  *mq;
    int          nx;
    pp_msg_node_t *node;

    mq = malloc( sizeof(pp_msg_que_t) + number*sizeof(pp_msg_node_t) + number*length );
    if (mq == NULL)
    {
        return NULL;    
    }    

    mq->number = number;
    mq->length = length;

    pp_slist_init( &(mq->queued_list) );
    pp_slist_init( &(mq->unqueued_list) );

    for (nx=0; nx<mq->number; nx++)
    {
        node = ((char *)mq) + sizeof(pp_msg_que_t) + nx*(sizeof(pp_msg_node_t)+mq->length);
        node->length = 0;
        pp_slist_put( &(mq->unqueued_list), node );    
    }

    return mq;
}

/*================================================================= pp_mq_show
 * description:
 * parameters:
 * returns:
 */

void pp_mq_show(pp_msg_que_t *mq)
{
    int          nx;
    pp_msg_node_t *node;

    pp_printf("0x%08x\n",mq);
    for (nx=0; nx<mq->number; nx++)
    {
        node = ((char *)mq) + sizeof(pp_msg_que_t) + nx*(sizeof(pp_msg_node_t)+mq->length);    
        pp_printf("0x%08x\n",node);
        pp_memory_dump16( (int)node );
    }
}

/*================================================================= pp_mq_send
 * description:
 * parameters:
 * returns:
 */

int pp_mq_send(pp_msg_que_t *mq,const char *message,int length)
{
    pp_msg_node_t *node;

    if (length <= 0 || length > mq->length)
    {
        return -1; /* error */
    }

    node = pp_slist_get( &(mq->unqueued_list) );
    if (node == NULL)
    {
        return -1; /* error */
    }

    memcpy( ((char *)node)+sizeof(pp_msg_node_t), message, length );

    node->length = length;

    pp_slist_put( &(mq->queued_list), node );

    return 0; /* success */
}

/*============================================================== pp_mq_receive
 * description:
 * parameters:
 * returns:
 */

int pp_mq_receive(pp_msg_que_t *mq,char *message,int length)
{
    pp_msg_node_t *node;
    int          lx;

    if (length <= 0)
    {
        return -1;
    }

    node = pp_slist_get( &(mq->queued_list) );
    if (node == NULL)
    {
        return -1;
    }

    lx = PP_MIN(length,node->length);

    memcpy( message, ((char *)node)+sizeof(pp_msg_node_t), lx );

    node->length = 0;

    pp_slist_put( &(mq->unqueued_list), node );

    return lx;
}

/*============================================================ pp_ht_index_for
 * description:
 * parameters:
 * returns:
 */

static unsigned int pp_ht_index_for(unsigned int tablelength, unsigned int hashvalue)
{
    /* from http://www.cl.cam.ac.uk/~cwc22/hashtable/hashtable.c */

    return (hashvalue % tablelength);
}


/*================================================================ pp_ht_hash2
 * description:
 * parameters:
 * returns:
 */

static unsigned int pp_ht_hash2(pp_hashtable_t *h, void *k)
{
    /* from http://www.cl.cam.ac.uk/~cwc22/hashtable/hashtable.c */

    /* Aim to protect against poor hash functions by adding logic here
     * - logic taken from java 1.4 hashtable source */
    unsigned int i = h->hashfn(k);
    i += ~(i << 9);
    i ^=  ((i >> 14) | (i << 18)); /* >>> */
    i +=  (i << 4);
    i ^=  ((i >> 10) | (i << 22)); /* >>> */
    return i;
}

/*=============================================================== pp_ht_expand
 * description:
 * parameters:
 * returns:
 */

static int pp_ht_expand(pp_hashtable_t *h)
{
    /* from http://www.cl.cam.ac.uk/~cwc22/hashtable/hashtable.c */

    /* Double the size of the table to accomodate more entries */
    pp_ht_entry_t     **newtable;
    pp_ht_entry_t      *e;
    pp_ht_entry_t     **pE;
    unsigned int      newsize;
    unsigned int      i;
    unsigned int      index;

    /* Check we're not hitting max capacity */
    if (h->primeindex == (pp_ht_prime_table_length - 1))
    {
        return -1; /* error */
    }

    newsize = pp_ht_primes[++(h->primeindex)];

    newtable = (pp_ht_entry_t **)malloc(sizeof(pp_ht_entry_t*) * newsize);
    if (NULL != newtable)
    {
        memset(newtable, 0, newsize * sizeof(pp_ht_entry_t *));
        /* This algorithm is not 'stable'. ie. it reverses the list
         * when it transfers entries between the tables */
        for (i = 0; i < h->tablelength; i++) {
            while (NULL != (e = h->table[i])) {
                h->table[i] = e->next;
                index = pp_ht_index_for(newsize,e->h);
                e->next = newtable[index];
                newtable[index] = e;
            }
        }
        free(h->table);
        h->table = newtable;
    }
    /* Plan B: realloc instead */
    else 
    {
        newtable = (pp_ht_entry_t **)
                   realloc(h->table, newsize * sizeof(pp_ht_entry_t *));
        if (NULL == newtable) { (h->primeindex)--; return 0; }
        h->table = newtable;
        memset(newtable[h->tablelength], 0, newsize - h->tablelength);
        for (i = 0; i < h->tablelength; i++) {
            for (pE = &(newtable[i]), e = *pE; e != NULL; e = *pE) {
                index = pp_ht_index_for(newsize,e->h);
                if (index == i)
                {
                    pE = &(e->next);
                }
                else
                {
                    *pE = e->next;
                    e->next = newtable[index];
                    newtable[index] = e;
                }
            }
        }
    }
    h->tablelength = newsize;
    h->loadlimit   = (unsigned int) ceil(newsize * pp_ht_max_load_factor);

    return 0; /* success */
}

/*=============================================================== pp_ht_create
 * description:
 * parameters:
 * returns:
 */

pp_hashtable_t *pp_ht_create(unsigned int minsize,
                           unsigned int (*hashf) (void*),
                           int (*eqf) (void*,void*))
{
    /* from http://www.cl.cam.ac.uk/~cwc22/hashtable/hashtable.c */

    pp_hashtable_t *h;
    unsigned int  pindex;
    unsigned int  size = pp_ht_primes[0];

    /* Check requested hashtable isn't too large */
    if (minsize > (1u << 30))
    {
        return NULL;
    }

    /* Enforce size as prime */
    for (pindex=0; pindex<pp_ht_prime_table_length; pindex++)
    {
        if (pp_ht_primes[pindex] > minsize)
        {
            size = pp_ht_primes[pindex];
            break;
        }
    }

    h = (pp_hashtable_t *)malloc(sizeof(pp_hashtable_t));
    if (NULL == h)
    {
        return NULL;
    }

    h->table = (pp_ht_entry_t **)malloc(sizeof(pp_ht_entry_t *) * size);
    if (NULL == h->table)
    {
        free(h);
        return NULL;
    }

    memset(h->table, 0, size * sizeof(pp_ht_entry_t *));
    h->tablelength  = size;
    h->primeindex   = pindex;
    h->entrycount   = 0;
    h->hashfn       = hashf;
    h->eqfn         = eqf;
    h->loadlimit    = (unsigned int) ceil(size * pp_ht_max_load_factor);

    return h;
}

/*================================================================ pp_ht_count
 * description:
 * parameters:
 * returns:
 */

unsigned int pp_ht_count(pp_hashtable_t *h)
{
    /* from http://www.cl.cam.ac.uk/~cwc22/hashtable/hashtable.c */

    return h->entrycount;
}

/*=============================================================== pp_ht_insert
 * description:
 * parameters:
 * returns:
 */

int pp_ht_insert(pp_hashtable_t *h, void *k, void *v)
{
    /* from http://www.cl.cam.ac.uk/~cwc22/hashtable/hashtable.c */

    /* This method allows duplicate keys - but they shouldn't be used */
    unsigned int   index;
    pp_ht_entry_t   *e;

    if (++(h->entrycount) > h->loadlimit)
    {
        /* Ignore the return value. If expand fails, we should
         * still try cramming just this value into the existing table
         * -- we may not have memory for a larger table, but one more
         * element may be ok. Next time we insert, we'll try expanding again.*/
        pp_ht_expand(h);
    }

    e = (pp_ht_entry_t *)malloc(sizeof(pp_ht_entry_t));
    if (NULL == e)
    {
        --(h->entrycount);
        return -1; /* error */
    }

    e->h = pp_ht_hash2(h,k);
    index = pp_ht_index_for(h->tablelength,e->h);
    e->k = k;
    e->v = v;
    e->next = h->table[index];
    h->table[index] = e;

    return 0; /* success */
}

/*=============================================================== pp_ht_search
 * description:
 * parameters:
 * returns:
 */

void * pp_ht_search(pp_hashtable_t *h, void *k)
{
    /* from http://www.cl.cam.ac.uk/~cwc22/hashtable/hashtable.c */

    pp_ht_entry_t  *e;
    unsigned int  hashvalue;
    unsigned int  index;

    if (h == NULL || k == NULL)
        return NULL;

    hashvalue = pp_ht_hash2(h,k);
    index = pp_ht_index_for(h->tablelength,hashvalue);
    e = h->table[index];
    while (NULL != e)
    {
        /* Check hash value to short circuit heavier comparison */
        if ((hashvalue == e->h) && (h->eqfn(k, e->k)))
        {
            return e->v;
        }
        e = e->next;
    }
    return NULL;
}

/*=============================================================== pp_ht_remove
 * description:
 * parameters:
 * returns:
 */

void * pp_ht_remove(pp_hashtable_t *h, void *k)
{
    return NULL; /* TBD */

    /* from http://www.cl.cam.ac.uk/~cwc22/hashtable/hashtable.c */

    /* TODO: consider compacting the table when the load factor drops enough,
     *       or provide a 'compact' method. */

    pp_ht_entry_t    *e;
    pp_ht_entry_t   **pE;
    void           *v;
    unsigned int    hashvalue;
    unsigned int    index;

    hashvalue = pp_ht_hash2(h,k);
    index = pp_ht_index_for(h->tablelength,pp_ht_hash2(h,k));
    pE = &(h->table[index]);
    e = *pE;
    while (NULL != e)
    {
        /* Check hash value to short circuit heavier comparison */
        if ((hashvalue == e->h) && (h->eqfn(k, e->k)))
        {
            *pE = e->next;
            h->entrycount--;
            v = e->v;
            free(e->k);
            free(e);
            return v;
        }
        pE = &(e->next);
        e = e->next;
    }

    return NULL;
}

/*======================================================== pp_ht_show_for_name
 * description:
 * parameters:
 * returns:
 */

void pp_ht_show_for_name(pp_hashtable_t *h,char *name)
{
    unsigned int   i;
    pp_ht_entry_t   *e;
    pp_ht_entry_t   *f;
    pp_ht_entry_t  **table = h->table;

    pp_printf("index value(&)   value(*)    key(&)     key(*)\n");
    for (i = 0; i < h->tablelength; i++)
    {
        e = table[i];
        while (NULL != e)
        {
            f = e;
            e = e->next;
            if (name == NULL || strstr(f->k,name) != NULL)
            {
                pp_printf("%04d  0x%08x 0x%08x  0x%08x %s\n",
                       pp_ht_index_for(h->tablelength,f->h),f->v,*(int *)f->v,f->k,f->k);
            }
        }
    }

    if (name == NULL)
    {
        pp_printf("table length %d ",h->tablelength);
        pp_printf("entry count %d ",h->entrycount);
        pp_printf("load limit %d ",h->loadlimit);
        pp_printf("prime index %d\n",h->primeindex);
    }
}

/*============================================================== pp_ht_destroy
 * description:
 * parameters:
 * returns:
 */

void pp_ht_destroy(pp_hashtable_t *h, int frpp_values)
{
    return; /* TBD */

    /* from http://www.cl.cam.ac.uk/~cwc22/hashtable/hashtable.c */

    unsigned int   i;
    pp_ht_entry_t   *e;
    pp_ht_entry_t   *f;
    pp_ht_entry_t  **table = h->table;

    if (frpp_values)
    {
        for (i = 0; i < h->tablelength; i++)
        {
            e = table[i];
            while (NULL != e)
            { f = e; e = e->next; free(f->k); free(f->v); free(f); }
        }
    }
    else
    {
        for (i = 0; i < h->tablelength; i++)
        {
            e = table[i];
            while (NULL != e)
            { f = e; e = e->next; free(f->k); free(f); }
        }
    }
    free(h->table);
    free(h);
}

/*============================================================ pp_name_compare
 * description:
 * parameters:
 * returns:
 */

static int pp_name_compare(char *name1,char *name2)
{
    return (strcmp(name1,name2) == 0);
}

/*=============================================================== pp_name_hash
 * description:
 * parameters:
 * returns:
 */

static unsigned int pp_name_hash(unsigned char *name)
{
    unsigned int hash = 0;
    int c;

    while (c = *name++)
        hash = c + (hash << 6) + (hash << 16) - hash;

    return hash;
}

/*==================================================== pp_name_get_nm_filename
 * description:
 * parameters:
 * returns:
 */

static char * pp_name_get_nm_filename()
{
    static char nm_filename[64];

#if defined(PP_CYGWIN)
    strcpy( nm_filename, getenv("_") );
    strcpy( strstr(nm_filename,".exe"), ".nm" );
#else
    strcpy( nm_filename, getenv("_") );
    strcat( nm_filename, ".nm" );
#endif

    return nm_filename;
}

/*======================================================== pp_name_get_from_nm
 * description:
 * parameters:
 * returns:
 */

static void * pp_name_get_from_nm(char *name)
{
    FILE *file;
    char  line[128];
    char *endptr;
    void *value = NULL;

    file = fopen(pp_name_get_nm_filename(),"r");
    if (file == NULL)
        return;

    while (fgets(line,sizeof(line),file) != NULL)
    {
        if (line[9] != 'T' && line[9] != 'D' && line[9] != 'B')
            continue;

        if (line[11+strlen(&line[11])-1] == 0x0a)
            line[11+strlen(&line[11])-1] = '\0';

#if defined(PP_CYGWIN)
        if (strcmp(&line[11+1],name) != 0)
        {
            /* _main, not main */
            continue;
        }
#else
        if (strcmp(&line[11],name) != 0)
        {
            /* main, not _main */
            continue;
        }
#endif

        line[8] = '\0';
        value = (void *)strtoul(&line[0],&endptr,16);    /* value = (void *)strtol(&line[0],&endptr,16); */

        break;
    }

    fclose(file);

    return value;
}

/*================================================================ pp_name_get
 * description:
 * parameters:
 * returns:
 */

void * pp_name_get(char *name)
{
    void *value;
    char *new_name;

    value = pp_ht_search(pp_names, name);
    if (value != NULL)
        return (int)(value);

    value = pp_name_get_from_nm(name);
    if (value != NULL)
    {
        new_name = strdup(name);    
        pp_name_put(new_name,value);
    }

    return value;
}

/*=============================================================== pp_name_show
 * description:
 * parameters:
 * returns:
 */

void pp_name_show(int fd,char *name)
{
    int value;

    if (name == NULL || name[0] == 0x00)
    {
        pp_ht_show_for_name(pp_names,NULL);
        return;
    }

    value = pp_name_get(name);
    if (value == NULL)
    {
        pp_printf("%s: unknown name\n",name);
        pp_ht_show_for_name(pp_names,name);
        return;
    }

    /* pp_memory_dump16( value ); */
    pp_printf("%d %d %d %f ",
           *(char *)value,*(short *)value,*(int *)value,*(float *)value);
    pp_printf("0x%02x 0x%04x 0x%08x at 0x%08x\n",
           *(char *)value,*(short *)value,*(int *)value,value);
}

/*======================================================== pp_name_show_stdout
 * description:
 * parameters:
 * returns:
 */

void pp_name_show_stdout(char *name)
{
    pp_name_show(1,name);
}

/*================================================================ pp_name_put
 * description:
 * parameters:
 * returns:
 */

void pp_name_put(char *name,void *value)
{
    if (pp_names == NULL)
    {
        pp_names = pp_ht_create(16,pp_name_hash,pp_name_compare);
        pp_name_put("nameshow",pp_name_show_stdout); /* recursive */
        pp_name_put("printf",printf); /* recursive */
    }

    pp_ht_insert(pp_names,name,value);
}

/*================================================================ pp_log_show
 * description:
 * parameters:
 * returns:
 */

static void pp_log_show()
{
    char  line[128];
    void *ring;
    int   nx;

    ring = pp_ring_duplicate(pp_log_ring);

    while (1)
    {
        nx = pp_ring_get_string(ring,line,sizeof(line)-3);
        if (nx <= 0)
            break;

        nx = PP_MIN(nx,strlen(line));

        line[nx]   = '\n';
        line[nx+1] = '\r';
        line[nx+2] = '\0';

        pp_printf(line);  /* write(fd,line,nx+2); */
    }

    pp_ring_delete(ring);
}

/*============================================================ pp_log_set_hook
 * description:
 * parameters:
 * returns:
 */

void pp_log_set_hook(void (*func)())
{
    pp_log_hook = func;
}

/*===================================================================== pp_log
 * description:
 * parameters:
 * returns:
 */

void pp_log(int priority, const char *format,int a,int b,int c,int d,int e,int f)
{
    time_t      current;
    char        line[256];
    int         nx;
    int         lx;
    int         length;

    if (pp_log_ring == NULL)
    {
        pp_log_ring = pp_ring_create(pp_log_ring_size);
        pp_name_put("logshow",pp_log_show);
    }

    /********************************/

    current = time(NULL);
    nx = sprintf( line,"%s%d:",ctime(&current),priority );
    line[24] = ':'; /* Tue May 19 17:59:13 2009\n */

    lx = sprintf( &line[nx],format,a,b,c,d,e,f );

    length = nx+lx;

    /********************************/

    if (pp_log_ring != NULL)
    {
        pp_ring_put_with_shift(pp_log_ring,&line[0],length+1); /* includes null */
    }
    else
    {
        line[length] = '\n';
        write(1,line,length+1);
    }

    /********************************/

    if (pp_log_hook)
    {
        pp_log_hook(line);
    }    
}

