/* pp_named_var.c - */

/*****************************************************************************
*
* Compiler will merge duplicate string literals with optimization.
*
* Have a look at this code:
*    const char* str0 = "Watchmen";
*    const char* str1 = "Watchmen";
*    char* str2 = "Watchmen";
*    char* str3 = "Watchmen";
* The variables will be like this:  
*    str0=0x443000 str1=0x443000 str2=0x443000 str3=0x443000
*
* Have a look at this code:
*    const char* str0 = "Watchmen";
*    const char* str1 = "atchmen"; 
*    char* str2 = "tchmen"; 
*    char* str3 = "chmen";
* The variables will be like this:  
*    str0=0x80487c0 str1=0x80487c1 str2=0x80487c2 str3=0x80487c3
*/

/************************************************************ include files */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

#include "pp_base.h"

/****************************************************************** defines */

#define pp_nlist_entry(node, type, list_member)   \
    ((type *)((char*)(node) - offsetof(type, list_member)))

#define PP_NHASH_LOW_WATER_MARK   2
#define PP_NHASH_HIGH_WATER_MARK  25
#define PP_NHASH_INITIAL_SIZE     29

#define PP_PQUEUE_INITIAL_SIZE   32

#define PP_NAMED_VAR_FLAG_UNUSED0001  0x0001
#define PP_NAMED_VAR_FLAG_UNUSED0002  0x0002
#define PP_NAMED_VAR_FLAG_UNUSED0004  0x0004
#define PP_NAMED_VAR_FLAG_NAMELESS    0x0008
#define PP_NAMED_VAR_FLAG_UNUSED0010  0x0010
#define PP_NAMED_VAR_FLAG_UNUSED0020  0x0020
#define PP_NAMED_VAR_FLAG_UNUSED0040  0x0040 
#define PP_NAMED_VAR_FLAG_UNUSED0080  0x0080
#define PP_NAMED_VAR_FLAG_STRING      0x0100
#define PP_NAMED_VAR_FLAG_INT         0x0200
#define PP_NAMED_VAR_FLAG_FLOAT       0x0400
#define PP_NAMED_VAR_FLAG_UNUSED0800  0x0800

/******************************************************************** types */

typedef struct pp_nlist_s
{
    struct pp_nlist_s   *next;
    struct pp_nlist_s   *prev;
    struct pp_nlist_s   *head;
    unsigned int         size;
} pp_nlist_t;

typedef unsigned int (*pp_nhash_obj_func)(const void *obj);

typedef unsigned int (*pp_nhash_key_func)(const void *key);

typedef int (*pp_nhash_cmp_func)(const void *obj, const void *key);

typedef void (*pp_nhash_foreach_cb_func)(void *obj, void *data);

typedef struct pp_nhash_s
{
    pp_nhash_obj_func   obj_hash_func;
    pp_nhash_key_func   key_hash_func;
    pp_nhash_cmp_func   obj_key_cmp;

    unsigned  elem;
    unsigned  size;
    void    **table;
} pp_nhash_t;

typedef int (*pp_pqueue_cmp_func)(const void *obj1, const void *obj2);

typedef void (*pp_pqueue_store_index)(void *obj, int indx);

typedef struct pp_pqueue_s
{
    pp_pqueue_cmp_func         cmp_func;

    pp_pqueue_store_index      store_index_func;

    int                         num_elem;
    int                         capacity;
    void                      **heap;
} pp_pqueue_t;

typedef void (*pp_pqueue_forech_cb_func)(void *obj, void *data);

#if defined(INCLUDE_PP_WAIT_QUEUE)
#endif /* defined(INCLUDE_PP_WAIT_QUEUE) */

enum pp_once_state_e {
    PP_ONCE_STATE_UNINITIALIZED,
    PP_ONCE_STATE_RUNNING,
    PP_ONCE_STATE_DONE
};

typedef struct pp_once_s
{
#if defined(INCLUDE_PP_WAIT_QUEUE)
#endif
    enum pp_once_state_e    state;
} pp_once_t;

#if 0
typedef struct pp_named_var_s
{
    const char *name;
    const char *value;
} pp_named_var_t;
#endif

typedef struct pp_named_var_entry_s
{
    short            flags;
    short            refcount;
    const char      *name;
    const char      *value;
} pp_named_var_entry_t;

typedef struct pp_named_var_tree_s
{
    pp_nhash_t         *vars;
    unsigned short      vr;
    int                 referenced;
} pp_named_var_tree_t;

typedef int (*pp_named_var_for_each_func)(const char *name, const char *value, short flags, void *cookie);

typedef struct pp_named_var_data_s
{
    void           *lock;
    pp_named_var_tree_t   tree[1];
} pp_named_var_data_t;

static pp_named_var_data_t   the_named_var;

#if defined(INCLUDE_PP_WAIT_QUEUE)
#else
static pp_once_t       the_named_var_once = { PP_ONCE_STATE_UNINITIALIZED };
#endif

#define PP_NAMED_VAR_CODE_LOCK()      /* pp_named_mutex_lock(the_named_var.lock) */
#define PP_NAMED_VAR_CODE_UNLOCK()    /* pp_named_mutex_unlock(the_named_var.lock) */

/************************************************ extern & local prototypes */

static int pp_nhash_add(pp_nhash_t *head, void *obj);

/********************************************************* extern & globals */

/******************************************************************* locals */

/*============================================================== pp_nlist_init
 * description:
 * parameters:
 * returns:
 */

#if 0 /* defined but not used */
static void pp_nlist_init(pp_nlist_t *head)
{
    head->next = head;
    head->prev = head;
    head->head = head;
    head->size = 0;
}
#endif

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

#if 0 /* defined but not used */
static pp_nlist_t *pp_nlist_first(pp_nlist_t *head)
{
    if(head->next != head)
        return head->next;
    else
        return NULL;
}
#endif

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

#if 0 /* defined but not used */
static pp_nlist_t *pp_nlist_last(pp_nlist_t *head)
{
    if(head->prev != head)
        return head->prev;
    else
        return NULL;
}
#endif

/*============================================================== pp_nlist_next
 * description:
 * parameters:
 * returns:
 */

#if 0 /* defined but not used */
static pp_nlist_t *pp_nlist_next(pp_nlist_t *entry)
{
    if(entry->next != entry->head)
        return entry->next;
    else
        return NULL;
}
#endif

/*============================================================== pp_nlist_prev
 * description:
 * parameters:
 * returns:
 */

#if 0 /* defined but not used */
static pp_nlist_t *pp_nlist_prev(pp_nlist_t *entry)
{
    if(entry->prev != entry->head)
        return entry->prev;
    else
        return NULL;
}
#endif

/*====================================================== pp_nlist_insert_first
 * description:
 * parameters:
 * returns:
 */

#if 0 /* defined but not used */
static void pp_nlist_insert_first(pp_nlist_t *head, pp_nlist_t *entry)
{
    entry->next = head->next;
    entry->prev = head;
    entry->next->prev = entry;
    entry->prev->next = entry;

    entry->head = head;
    head->size++;
}
#endif

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

#if 0 /* defined but not used */
static void pp_nlist_insert_last(pp_nlist_t *head, pp_nlist_t *entry)
{
    entry->prev = head->prev;
    entry->next = head;
    entry->next->prev = entry;
    entry->prev->next = entry;

    entry->head = head;
    head->size++;
}
#endif

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

#if 0 /* defined but not used */
static void pp_nlist_insert_before(pp_nlist_t *inlist, pp_nlist_t *entry)
{
    entry->next = inlist;
    entry->prev = inlist->prev;

    entry->next->prev = entry;
    entry->prev->next = entry;

    entry->head = inlist->head;
    entry->head->size++;
}
#endif

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

#if 0 /* defined but not used */
static void pp_nlist_insert_after(pp_nlist_t *inlist, pp_nlist_t *entry)
{
    entry->prev = inlist;
    entry->next = inlist->next;

    entry->next->prev = entry;
    entry->prev->next = entry;

    entry->head = inlist->head;
    entry->head->size++;
}
#endif

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

#if 0 /* defined but not used */
static void pp_nlist_remove(pp_nlist_t *entry)
{
    entry->next->prev = entry->prev;
    entry->prev->next = entry->next;

    entry->head->size--;
    entry->head = NULL;
}
#endif

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

#if 0 /* defined but not used */
static pp_nlist_t *pp_nlist_unlink_head(pp_nlist_t *head)
{
    pp_nlist_t   *new_head;

    new_head = head->next;
    if(new_head == head)
        return NULL;

    new_head->head = new_head;
    new_head->size = head->size - 1;
    new_head->prev = head->prev;
    new_head->prev->next = new_head;

    for(head = new_head->next; head != new_head; head = head->next)
        head->head = new_head;

    return new_head;
}
#endif

/*============================================================ pp_nhash_resize
 * description:
 * parameters:
 * returns:
 */

static int pp_nhash_resize(pp_nhash_t *head, int increase)
{
    void **old_hash_table;
    int    old_size;
    int    i;
    int err;

    old_hash_table = head->table;
    old_size = head->size;

    if (increase)
        head->size = (head->size << 1) + 3;
    else
        head->size = (head->size >> 1) - 1;

    head->table = calloc(head->size, sizeof(*head->table));
    if (head->table == NULL)
    {
        head->size = old_size;
        head->table = old_hash_table;
        return -1;
    }

    head->elem = 0;
    for (i = 0; i < old_size; i++)
        if (old_hash_table[i] != NULL)
        {
            err = pp_nhash_add(head, old_hash_table[i]);
            (void)err;
        }

    free(old_hash_table);
    return 0;
}

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

static int pp_nhash_can_move_obj(unsigned ideal_index, unsigned new_index, unsigned size)
{
    int half_size = size >> 1;

#if PP_NHASH_HIGH_WATER_MARK > 50
    /* NOTE: this algorithm will break if PP_NHASH_HIGH_WATER_MARK
       is 50 or higher */
#endif

    if (abs((int) (ideal_index - new_index)) > half_size)
    {
        ideal_index = (ideal_index + half_size) % size;
        new_index = (new_index + half_size) % size;
    }

    return ideal_index <= new_index;
}

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

static pp_nhash_t * pp_nhash_new(pp_nhash_obj_func obj_hash_func,
               pp_nhash_key_func key_hash_func,
               pp_nhash_cmp_func obj_key_cmp)
{
    pp_nhash_t *head;

    head = malloc(sizeof(*head));
    if (head == NULL)
        return NULL;
    head->table = calloc(PP_NHASH_INITIAL_SIZE, sizeof(*head->table));
    if (head->table == NULL)
    {
        free(head);
        return NULL;
    }

    head->obj_hash_func = obj_hash_func;
    head->key_hash_func = key_hash_func;
    head->obj_key_cmp = obj_key_cmp;
    head->size = PP_NHASH_INITIAL_SIZE;
    head->elem = 0;
    return head;
}

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

#if 0 /* defined but not used */
static void pp_nhash_delete(pp_nhash_t *head)
{
    if (head == NULL)
        return;
    head->size = 0;
    free(head->table);
    free(head);
}
#endif

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

static int pp_nhash_add(pp_nhash_t *head, void *obj)
{
    unsigned hash_index;
    int   err;

    if ((head->elem + 1) * 100 / head->size > PP_NHASH_HIGH_WATER_MARK)
    {
        err = pp_nhash_resize(head, 1);
        if (err != 0)
            return err;
    }

    if (head->size / 2 <= head->elem)
        /* The size of the hash table is locked and it has reached
           the capacity limit */
        return -1;

    hash_index = head->obj_hash_func(obj) % head->size;
    /* Find an empty slot at the hash index or after it if another
       object hash:ed to the same value */
    while (head->table[hash_index % head->size] != NULL)
    {
        if (head->table[hash_index % head->size] == obj)
            return -1;
        hash_index++;
    }
    head->table[hash_index % head->size] = obj;
    head->elem++;
    return 0;
}

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

static int pp_nhash_remove(pp_nhash_t *head, void *obj)
{
    void    *cobj;
    unsigned hash_index;
    unsigned new_index;

    hash_index = head->obj_hash_func(obj) % head->size;
    while (head->table[hash_index] != obj)
    {
        if (head->table[hash_index] == NULL)
            /* The object does not exist */
            return -1;
        hash_index++;
        hash_index = hash_index % head->size;
    }
    /* Object found, remove it */
    head->table[hash_index] = NULL;
    head->elem--;

    if (head->size > PP_NHASH_INITIAL_SIZE &&
        head->elem * 100 / head->size < PP_NHASH_LOW_WATER_MARK)
    {
        /* Half the size of the hash table */
        if (pp_nhash_resize(head, 0) == 0)
            /* No need to fix collisions since the hash table was regenerated */
            return 0;
    }

    new_index = hash_index;
    /* The following object(s) can be out-of-place due to collisions, check them */
    while ((cobj = head->table[++hash_index % head->size]) != NULL)
    {
        unsigned ideal_index = head->obj_hash_func(cobj) % head->size;

        hash_index %= head->size;
        if (pp_nhash_can_move_obj(ideal_index, new_index, head->size))
        {
            /* Move this object to the hole from the removed or last moved object */
            head->table[new_index] = cobj;
            head->table[hash_index] = NULL;
            new_index = hash_index;
        }
        /* else: the object is already at the correct place */
    }
    return 0;
}

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

static void * pp_nhash_get(pp_nhash_t *head, const void *key)
{
    unsigned hash_index;
    void    *obj;

    hash_index = head->key_hash_func(key) % head->size;
    while ((obj = head->table[hash_index]) != NULL)
    {
        if (head->obj_key_cmp(obj, key))
            return obj;
        hash_index = (hash_index + 1) % head->size;
    }
    return NULL;
}

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

static void pp_nhash_for_each(pp_nhash_t *head, pp_nhash_foreach_cb_func cb_func, void *data)
{
    unsigned  i;
    unsigned  x;
    void    **elems;

    if (head->elem == 0)
        return;

    elems = malloc(head->elem * sizeof(void*));
    if (elems == NULL)
    {
        return;
    }

    for (i = 0, x = 0; i < head->size; i++)
        if (head->table[i] != NULL)
            elems[x++] = head->table[i];

    for (i = 0; i < x; i++)
        cb_func(elems[i], data);

    free(elems);
}

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

static unsigned pp_nhash_update(const void *key_param, int key_len, unsigned initval)
{
   unsigned a;
   unsigned len;
   unsigned char *key = (unsigned char *) key_param;

   /* Set up the internal state */
   len = (unsigned) key_len;
   a = initval + len;

   while (len >= 4)
   {
       a += key[0] + (key[1] << 8) + (key[2] << 16) + (key[3] << 24);
       a += ~(a << 15);
       a ^=  (a >> 10);
       a +=  (a << 3);
       a ^=  (a >> 6);
       a += ~(a << 11);
       a ^=  (a >> 16);
       key += 4; len -= 4;
   }

   /* All the case statements fall through */
   switch (len)
   {
   case 3 : a += key[2] << 16;
   case 2 : a ^= key[1] << 8;
   case 1 : a += key[0];
   default: break;
   }

   return a;
}

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

static int pp_pqueue_upheap(pp_pqueue_t *pqueue, int i)
{
    int *obj;
    int new_i;

    obj = pqueue->heap[i];
    for (;;)
    {
        new_i = i >> 1;
        if (new_i == 0 || pqueue->cmp_func(pqueue->heap[new_i], obj) > 0)
            break;

        pqueue->heap[i] = pqueue->heap[new_i];
        pqueue->store_index_func(pqueue->heap[i], i);
        i = new_i;
    }
    pqueue->heap[i] = obj;
    pqueue->store_index_func(obj, i);
    return i;
}

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

static void pp_pqueue_downheap(pp_pqueue_t *pqueue, int i)
{
    int *obj;
    int new_i;
    int max_i = pqueue->num_elem >> 1;

    obj = pqueue->heap[i];
    while (i <= max_i)
    {
        new_i = i << 1;
        if (new_i < pqueue->num_elem
            && pqueue->cmp_func(pqueue->heap[new_i], pqueue->heap[new_i + 1]) < 0)
        {
            new_i++;
        }

        if (pqueue->cmp_func(obj, pqueue->heap[new_i]) >= 0)
            goto done;

        pqueue->heap[i] = pqueue->heap[new_i];
        pqueue->store_index_func(pqueue->heap[i], i);
        i = new_i;
    }

 done:
    pqueue->heap[i] = obj;
    pqueue->store_index_func(obj, i);
}

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

static pp_pqueue_t * pp_pqueue_new(pp_pqueue_cmp_func cmp_func, pp_pqueue_store_index store_index_func)
{
    pp_pqueue_t *pqueue;

    pqueue = calloc(1, sizeof(pp_pqueue_t));
    if (pqueue != NULL)
    {
        pqueue->heap = malloc((PP_PQUEUE_INITIAL_SIZE + 1) * sizeof(void*));
        if (pqueue->heap == NULL)
        {
            free(pqueue);
            return NULL;
        }
        pqueue->capacity         = PP_PQUEUE_INITIAL_SIZE;
        pqueue->cmp_func         = cmp_func;
        pqueue->store_index_func = store_index_func;
    }
    return pqueue;
}

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

static void pp_pqueue_delete(pp_pqueue_t *pqueue)
{
    free(pqueue->heap);
    free(pqueue);
}

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

static int pp_pqueue_insert(pp_pqueue_t *pqueue, void *obj)
{
    if (pqueue->num_elem + 1 > pqueue->capacity)
    {
        void *heap_ptr = realloc(pqueue->heap, ((pqueue->capacity << 1) + 1) * sizeof(void*));
        if (heap_ptr == NULL)
            return -1;
        pqueue->heap = heap_ptr;
        pqueue->capacity <<= 1;
    }

    pqueue->num_elem++;
    pqueue->heap[pqueue->num_elem] = obj;
    (void)pp_pqueue_upheap(pqueue, pqueue->num_elem);
    return 0;
}

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

static void * pp_pqueue_remove(pp_pqueue_t *pqueue, int pindex)
{
    void *obj;

    if (pindex > pqueue->num_elem)
        return NULL;

    obj = pqueue->heap[pindex];
    pqueue->store_index_func(obj, 0);
    if (pindex < pqueue->num_elem--)
    {
        pqueue->heap[pindex] = pqueue->heap[pqueue->num_elem + 1];
        pqueue->store_index_func(pqueue->heap[pindex], pindex);
        pp_pqueue_downheap(pqueue, pp_pqueue_upheap(pqueue, pindex));
    }
    return obj;
}

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

static void * pp_pqueue_remove_next(pp_pqueue_t *pqueue)
{
    return pp_pqueue_remove(pqueue, 1);
}

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

#if 0 /* defined but not used */
static void * pp_pqueue_get(pp_pqueue_t *pqueue, int pindex)
{
    void *obj;

    if (pindex > pqueue->num_elem)
        return NULL;
    obj = pqueue->heap[pindex];
    return obj;
}
#endif

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

#if 0 /* defined but not used */
static void * pp_pqueue_get_next(pp_pqueue_t *pqueue)
{
    if (pqueue->num_elem == 0)
        return NULL;
    return pqueue->heap[1];
}
#endif

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

#if 0 /* defined but not used */
static void pp_pqueue_for_each(pp_pqueue_t *pqueue, pp_pqueue_forech_cb_func cb_func, void *data)
{
    int i;

    for (i = 1; i <= pqueue->num_elem; i++)
        cb_func(pqueue->heap[i], data);
}
#endif

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

#if 0 /* defined but not used */
static int pp_pqueue_size(pp_pqueue_t *pqueue)
{
    return pqueue->num_elem;
}
#endif

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

static void pp_pqueue_nop_store_index(void *obj, int indx)
{
    (void) obj;
    (void) indx;
}

#if defined(INCLUDE_PP_WAIT_QUEUE)
#endif /* defined(INCLUDE_PP_WAIT_QUEUE) */


#if defined(INCLUDE_PP_WAIT_QUEUE)
#endif

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

static int pp_once(pp_once_t *once, int (*init)(void *param), void *param)
{
#if defined(INCLUDE_PP_WAIT_QUEUE)
#else
    int ret = 0; /* success */

    if (once->state == PP_ONCE_STATE_DONE)
        return 0;

    while (once->state == PP_ONCE_STATE_RUNNING)
        pp_thread_delay(10);

    if (once->state == PP_ONCE_STATE_UNINITIALIZED)
    {
        once->state = PP_ONCE_STATE_RUNNING;
        ret = init(param);
        once->state = PP_ONCE_STATE_DONE;
    }

    return ret;
#endif
}

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

static unsigned pp_named_var_hash_key(const char *key)
{
    return pp_nhash_update(key, strlen(key), 0);
}

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

static unsigned pp_named_var_hash_obj(pp_named_var_entry_t *s)
{
    return pp_named_var_hash_key(s->name);
}

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

static int pp_named_var_hash_cmp(pp_named_var_entry_t *s, char *key)
{
    return strcmp(s->name, key) == 0;
}

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

static int pp_named_var_pqueue_cmp(pp_named_var_entry_t *s1, pp_named_var_entry_t *s2)
{
    return strcmp(s2->name, s1->name);
}

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

static void pp_named_var_pqueue_insert_cb(pp_named_var_entry_t *s, pp_pqueue_t *pq)
{
    s->refcount++;
    pp_pqueue_insert(pq, s);
}

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

static pp_named_var_tree_t * pp_named_var_tree_get(int vr)
{
    vr = 0;

    if (the_named_var.tree[vr].vars)
    {
        the_named_var.tree[vr].referenced++;
    }
    else
    {
        the_named_var.tree[vr].vars
            = pp_nhash_new((pp_nhash_obj_func) pp_named_var_hash_obj,
                             (pp_nhash_key_func) pp_named_var_hash_key,
                             (pp_nhash_cmp_func) pp_named_var_hash_cmp);

        if (the_named_var.tree[vr].vars == NULL)
            return NULL;

        the_named_var.tree[vr].vr = vr;
        the_named_var.tree[vr].referenced = 1;
    }

    return &the_named_var.tree[vr];
}

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

static void pp_named_var_tree_done(pp_named_var_tree_t    *tree)
{
    if (tree == NULL)
        return;

    /* If we're killing the last reference, to deal finalization */
    if (tree->referenced == 1)
    {
        /* Did it kill the tree? */
    }

    /* Decrease AFTER, since finalize may invoke var calls as
       well */
    tree->referenced--;
}

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

static int pp_named_var_init2(void *ununsed)
{
    if (sizeof(char *) != sizeof(int))
        pp_thread_panic();
    if (sizeof(char *) != sizeof(float))
        pp_thread_panic();

    memset(&the_named_var, 0, sizeof(pp_named_var_data_t));
    return 0; /* return pp_named_mutex_create(&the_named_var.lock); */
}

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

void pp_named_var_init()
{
}

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

static void pp_named_var_release(pp_named_var_tree_t *tree, pp_named_var_entry_t *var)
{
    if (--var->refcount == 0)
    {
        pp_nhash_remove(tree->vars, var);
        free(var);
    }
}

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

static char * pp_named_var_getvr(const char *name, char *value, int *value_size, int pointer, int vr)
{
    pp_named_var_entry_t  *var;
    int               value_length;
    char             *ret_value = NULL;
    int               retval;
    pp_named_var_tree_t   *tree;

    if (name == NULL)
        return NULL;

    retval = pp_once(&the_named_var_once, pp_named_var_init2, NULL);
    if (retval != 0)
    {
        return NULL;
    }

    PP_NAMED_VAR_CODE_LOCK();

    tree = pp_named_var_tree_get(vr);
    if (tree == NULL)
        goto leave;

    var = pp_nhash_get(tree->vars, name);
    if (var == NULL)
        goto leave;

    if (pointer)
    {
        ret_value = (char *)&var->value;
        goto leave;
    }

    if (var->flags & PP_NAMED_VAR_FLAG_STRING)
    {
        if (value == NULL)
        {
                ret_value = strdup(var->value);
        }
        else
        {
            value_length = strlen(var->value) + 1;
            if (value_length > *value_size)
            {
                ret_value = NULL;
                *value_size = value_length;
            }
            else
            {
                ret_value = value;
                memcpy(ret_value, var->value, value_length);
                *value_size = value_length - 1;
            }
        }
    }
    else if (var->flags & PP_NAMED_VAR_FLAG_INT)
    {
        ret_value = (char *)var->value;
    }
    else if (var->flags & PP_NAMED_VAR_FLAG_FLOAT)
    {
        ret_value = (char *)var->value;
    }
    else
    {
        ret_value = (char *)var->value;
    }

 leave:
    pp_named_var_tree_done(tree);
    PP_NAMED_VAR_CODE_UNLOCK();
    return ret_value;
}

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

static int pp_named_var_set_tree(const char *name, const char *value, short flags, pp_named_var_tree_t *tree)
{
    int               name_length;
    int               value_length;
    pp_named_var_entry_t  *var;
    int               new_name = 0; /* 0 is false */
    int               retval = 0; /* 0 is success */

    var = pp_nhash_get(tree->vars, name);
    if (var)
    {
        if (var->flags != flags)
        {
            retval = -1;
            goto leave;
        }
        else if (flags & PP_NAMED_VAR_FLAG_STRING)
        {
            /* do not check if strlen(var->value) < strlen(value) */
            pp_nhash_remove(tree->vars, var);
            free(var);
            var = NULL;
        }
    }

    if (flags & PP_NAMED_VAR_FLAG_NAMELESS)
        name_length = 0;
    else     
        name_length = strlen(name) + 1;

    if (flags & PP_NAMED_VAR_FLAG_STRING)
        value_length = strlen(value) + 1;
    else if (flags & PP_NAMED_VAR_FLAG_INT)
        value_length = 0;
    else if (flags & PP_NAMED_VAR_FLAG_FLOAT)
        value_length = 0;
    else
        value_length = 0;

    if (var == NULL)
    {
        var = malloc(sizeof(pp_named_var_entry_t) + name_length + value_length);
        if(var == NULL)
        {
            retval = -1;
            goto leave;
        }

        if (name_length)
        {
            var->name = (const char *)var + sizeof(pp_named_var_entry_t);
            memcpy((void *)var->name, name, name_length);
        }
        else
        {
           var->name = name;
        }

        new_name = 1; /* true */
    }

    var->refcount = 1;
    var->flags    = flags;

    if (value_length)
    {
        var->value = var->name + name_length;
        memcpy((void *)var->value, value, value_length);
    }
    else
    {
        var->value = value;
    }

    if (new_name)
        retval = pp_nhash_add(tree->vars, var);

leave:
    return retval;
}

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

static int pp_named_var_setvr(const char *name, const char *value, short flags, int vr)
{
    int retval;
    pp_named_var_tree_t   *tree;


    if (value == NULL)
        return 0;  /* simply ignore it silently */

    retval = pp_once(&the_named_var_once, pp_named_var_init2, NULL);
    if (retval != 0)
    {
        return retval;
    }

    PP_NAMED_VAR_CODE_LOCK();

    tree = pp_named_var_tree_get(vr);
    if (tree == NULL)
        retval = -1;
    else
        retval =  pp_named_var_set_tree(name, value, flags, tree);
    pp_named_var_tree_done(tree);

    PP_NAMED_VAR_CODE_UNLOCK();

    return retval;
}

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

static char * pp_named_var_get_ptr(const char *name)
{
    return pp_named_var_getvr(name, NULL, 0, 1, -1);
}

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

static char * pp_named_var_get(const char *name, char *value, int *value_size)
{
    return pp_named_var_getvr(name, value, value_size, 0, -1);
}

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

#if 0 /* defined but not used */
static char * pp_named_var_get0(const char *name, char *value, int *value_size)
{
    return pp_named_var_getvr(name, value, value_size, 0, 0);
}

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

static int pp_named_var_get_descriptive_int0(const char *name,
                                  int default_value,
                                  int *value,
                                  const void *map)
{
    char      strval[32];
    int strval_len = sizeof(strval);

    *value = default_value;
    if (pp_named_var_get0(name, strval, &strval_len) == NULL)
        return -1;

    *value = atoi(strval);
    return 1;
}

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

static int pp_named_var_get_as_int0(const char *name, int default_value)
{
    int val;
    (void)pp_named_var_get_descriptive_int0(name, default_value, &val, NULL);
    return val;
}

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

static int pp_named_var_set0(const char *name, const char *value, short flags)
{
    return pp_named_var_setvr(name, value, flags, 0);
}
#endif

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

static int pp_named_var_set(const char *name, const char *value, short flags)
{
    return pp_named_var_setvr(name, value, flags, -1);
}

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

#if 0
int pp_named_var_setv(const pp_named_var_t entries[], short flags)
{
    int ix;

    for (ix=0; entries[ix].name != NULL; ix++)
        ;

    for (--ix; ix>=0; ix--)
    {
        (void)pp_named_var_set(entries[ix].name,entries[ix].value,flags);
    }    

    return 0;
}
#endif

/*====================================================== pp_named_var_for_each
 * description:
 * parameters:
 * returns:
 */

static int pp_named_var_for_each(const char *name, pp_named_var_for_each_func cb_func, void *cookie)
{
    pp_named_var_entry_t *var;
    pp_named_var_tree_t  *tree;
    int              namelen = 0;
    int              retval;
    pp_pqueue_t    *pq;

    retval = pp_once(&the_named_var_once, pp_named_var_init2, NULL);
    if (retval != 0)
        return retval;

    if (name)
    {
        namelen = strlen(name);
        if (name[namelen-1] == '*')
            namelen--; /* ignore trailing wildcard sign */
    }

    pq = pp_pqueue_new((pp_pqueue_cmp_func)pp_named_var_pqueue_cmp,
                          pp_pqueue_nop_store_index);
    if (pq == NULL)
        return -1;

    PP_NAMED_VAR_CODE_LOCK();

    tree = pp_named_var_tree_get(-1);
    if (tree == NULL)
    {
        pp_pqueue_delete(pq);
        PP_NAMED_VAR_CODE_UNLOCK();
        return -1;
    }

    /* Add all vars to the priority queue so they can be extracted
       sorted in lexicographical order */
    pp_nhash_for_each(tree->vars,
                        (pp_nhash_foreach_cb_func)pp_named_var_pqueue_insert_cb,
                        pq);

    while (NULL != (var = pp_pqueue_remove_next(pq)))
    {
        if (name == NULL || strncmp(var->name, name, namelen) == 0)
        {
            PP_NAMED_VAR_CODE_UNLOCK();
            (void)cb_func(var->name, var->value, var->flags, cookie);
            PP_NAMED_VAR_CODE_LOCK();
        }
        pp_named_var_release(tree, var);
    }

    pp_pqueue_delete(pq);
    pp_named_var_tree_done(tree);

    PP_NAMED_VAR_CODE_UNLOCK();

    return 0;
}

/*==================================================== pp_named_var_set_as_str
 * description:
 * parameters:
 * returns:
 */

int pp_named_var_set_as_str(const char *name, const char *value)
{
    short flags = PP_NAMED_VAR_FLAG_STRING;
    return pp_named_var_set(name, value, flags);
}

/*==================================================== pp_named_var_set_as_int
 * description:
 * parameters:
 * returns:
 */

int pp_named_var_set_as_int(const char *name, int value)
{
    short   flags = PP_NAMED_VAR_FLAG_INT;
    return pp_named_var_set(name, (char *)value, flags);
}

/*================================================== pp_named_var_set_as_float
 * description:
 * parameters:
 * returns:
 */

int pp_named_var_set_as_float(const char *name, float value)
{
    short  flags = PP_NAMED_VAR_FLAG_FLOAT;
    char   *line;

    memcpy(&line,&value,4);

    return pp_named_var_set(name, line, flags);
}

/*==================================================== pp_named_var_get_as_str
 * description:
 * parameters:
 * returns:
 */

char * pp_named_var_get_as_str(const char *name, char *value, int *value_size)
{
    return pp_named_var_get(name, value, value_size);
}

/*==================================================== pp_named_var_get_as_int
 * description:
 * parameters:
 * returns:
 */

int pp_named_var_get_as_int(const char *name)
{
    return (int)pp_named_var_get(name,NULL,0);
}

/*================================================== pp_named_var_get_as_float
 * description:
 * parameters:
 * returns:
 */

float pp_named_var_get_as_float(const char *name)
{
    char *ret_value;
    float fx;

    ret_value = pp_named_var_get(name,NULL,0);
    memcpy(&fx,&ret_value,4);

    return fx;
}

/*================================================ pp_named_var_get_as_str_ptr
 * description:
 * parameters:
 * returns:
 */

char *pp_named_var_get_as_str_ptr(const char *name)
{
    return (char *)pp_named_var_get_ptr(name);
}

/*================================================ pp_named_var_get_as_int_ptr
 * description:
 * parameters:
 * returns:
 */

int *pp_named_var_get_as_int_ptr(const char *name)
{
    return (int *)pp_named_var_get_ptr(name);
}

/*============================================== pp_named_var_get_as_float_ptr
 * description:
 * parameters:
 * returns:
 */

float *pp_named_var_get_as_float_ptr(const char *name)
{
    return (float *)pp_named_var_get_ptr(name);
}

/*============================================ pp_named_var_printf_for_each_cb
 * description:
 * parameters:
 * returns:
 */

static int pp_named_var_printf_for_each_cb(const char *name, const char *value, short flags, void *cookie)
{
    if (flags & PP_NAMED_VAR_FLAG_STRING)
    {
        printf("%s=%s""\n", name, value);
    }
    else if (flags & PP_NAMED_VAR_FLAG_INT)
    {
        printf("%s=%d""\n", name, (int)value);
    }
    else if (flags & PP_NAMED_VAR_FLAG_FLOAT)
    {
        float fx;
        memcpy(&fx,&value,4);
        printf("%s=%16.8f""\n", name, fx);
    }
    return 0;
}

/*========================================================== pp_named_var_list
 * description:
 * parameters:
 * returns:
 */

void pp_named_var_list(char *name)
{
    pp_named_var_for_each(name, pp_named_var_printf_for_each_cb, NULL);
}

/*======================================================== pp_named_var_is_int
 * description:
 * parameters:
 * returns:
 */

static int pp_named_var_is_int(const char *value)
{
    const char *px = value;

    if (*px == '-')
        px++;

    while (*px)
    {
        if ('0' <= *px && *px <= '9')
            px++;
        else
            return 0; /* false */
    }

    return 1; /* true */
}

/*====================================================== pp_named_var_is_float
 * description:
 * parameters:
 * returns:
 */

static int pp_named_var_is_float(const char *value)
{
    const char *px = value;
    int         dotted = 0;

    if (*px == '-')
        px++;

    while (*px)
    {
        if ('0' <= *px && *px <= '9')
        {
            px++;
        }
        else if('.' == *px)
        {
            px++;
            dotted++;
        }
        else
        {
            return 0; /* false */
        }
    }

    if (dotted != 1)
        return 0; /* false */

    return 1; /* true */
}

/*===================================================== pp_named_var_from_file
 * description:
 * parameters:
 * returns:
 */

void pp_named_var_from_file(const char *filename)
{
    FILE    *from_file;
    char     line[128];
    char    *separator = "= \t\n\r";
    char    *last;
    char    *name;
    char    *value;

    from_file = fopen(filename,"r");

    while (fgets(line,sizeof(line)-1,from_file))
    {
        name = strtok_r(line,separator,&last);
        value = strtok_r(NULL,separator,&last);
        if (name == NULL || value == NULL)
            continue;

        if (pp_named_var_is_int(value))
            pp_named_var_set_as_int(name,atoi(value));
        else if (pp_named_var_is_float(value))
            pp_named_var_set_as_float(name,atof(value));
        else
            pp_named_var_set_as_str(name,value);
    }

    fclose(from_file);
}

/*=========================================== pp_named_var_fprintf_for_each_cb
 * description:
 * parameters:
 * returns:
 */

static int pp_named_var_fprintf_for_each_cb(const char *name, const char *value, short flags, void *cookie)
{
    FILE *to_file = (FILE *)cookie;

    if (flags & PP_NAMED_VAR_FLAG_STRING)
    {
        fprintf(to_file,"%s=%s""\n", name, value);
    }
    else if (flags & PP_NAMED_VAR_FLAG_INT)
    {
        fprintf(to_file,"%s=%d""\n", name, (int)value);
    }
    else if (flags & PP_NAMED_VAR_FLAG_FLOAT)
    {
        float fx;
        memcpy(&fx,&value,4);
        fprintf(to_file,"%s=%16.8f""\n", name, fx);
    }

    return 0;
}

/*======================================================= pp_named_var_to_file
 * description:
 * parameters:
 * returns:
 */

void pp_named_var_to_file(const char *filename)
{
    FILE *to_file;

    to_file = fopen(filename,"w");

    pp_named_var_for_each(NULL, pp_named_var_fprintf_for_each_cb, to_file);

    fclose(to_file);
}

