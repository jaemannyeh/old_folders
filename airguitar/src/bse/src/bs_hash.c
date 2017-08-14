
#include "bs_hash.h"

#include <stdio.h>
#include <stdlib.h>

#define BS_HASH_INITIAL_SIZE 3571 /* 3571 X 4 = 14284 bytes */


static bs_err bs_hash_resize(bs_hash_t *head)
{
    int    old_size;
    void **old_hash_table;
    int    ex;
    
    old_size = head->size;
    old_hash_table = head->table;

    head->size = (head->size << 1) + 3; /* should be odd */
    head->table = calloc(head->size, sizeof(*head->table)); /* zero-fill */
    if (head->table == NULL)
    {
        head->size = old_size;
        head->table = old_hash_table;
        return -1;
    }

    head->elem = 0;
    for (ex = 0; ex < old_size; ex++)
    {
        if (old_hash_table[ex] != NULL)
        {
            (void) bs_hash_add(head, old_hash_table[ex]);
        }
    }
    
    free(old_hash_table);
    return 0;
}

static bs_bool bs_hash_can_move_obj(unsigned int ideal_index, unsigned int new_index, unsigned int size)
{
    int half_size = size >> 1;
    
    if (abs((int) (ideal_index - new_index)) > half_size)
    {
        ideal_index = (ideal_index + half_size) % size;
        new_index = (new_index + half_size) % size;
    }
    
    return ideal_index <= new_index;
}

unsigned int bs_hash_miss(bs_hash_t *head)
{
    return head->miss;
}

unsigned int bs_hash_elem(bs_hash_t *head)
{
    return head->elem;
}

unsigned int bs_hash_size(bs_hash_t *head)
{
    return head->size;
}

bs_hash_t *bs_hash_new(bs_hash_obj_f obj_hash_func,bs_hash_key_f key_hash_func,bs_hash_cmp_f obj_key_cmp)
{
    bs_hash_t *head;

    head = malloc(sizeof(*head));
    if (head == NULL)
        return NULL;
    
    head->table = calloc(BS_HASH_INITIAL_SIZE, sizeof(*head->table)); /* zero-fill */
    if (head->table == NULL)
    {
        free(head);
        return NULL;
    }

    head->obj_hash_func = obj_hash_func;
    head->key_hash_func = key_hash_func;
    head->obj_key_cmp = obj_key_cmp;
    head->miss = 0;
    head->elem = 0;
    head->size = BS_HASH_INITIAL_SIZE;

    return head;    
}

void bs_hash_delete()
{
    /* not supported */
}

bs_err bs_hash_add(bs_hash_t *head,void *obj)
{
    unsigned int hash_index;
    unsigned int hash_miss;

    if ((head->elem + 1) * 100 / head->size > 25) /* 25 % */
    {
        if (0 != bs_hash_resize(head))
            return -1;
    }

    if (head->size / 2 <= head->elem)
    {
        return -1; /* reached the limit */
    }

	hash_miss = 0;
    hash_index = head->obj_hash_func(obj) % head->size;
    while (head->table[hash_index % head->size] != NULL)
    {
        if (head->table[hash_index % head->size] == obj)
        {
            return -1; /* duplicated */
        }            
        hash_index++;
		hash_miss++;
    }
    head->table[hash_index % head->size] = obj;
    head->elem++;
    head->miss = hash_miss;
        		
    return 0; /* success */
}

bs_err bs_hash_remove(bs_hash_t *head,void *obj)
{
    unsigned int  hash_index;
    unsigned int  new_index;
    unsigned int  ideal_index;
    void         *cobj;    

    hash_index = head->obj_hash_func(obj) % head->size;
    while (head->table[hash_index] != obj)
    {
        if (head->table[hash_index] == NULL)
            return -1; /* not found */   
        hash_index = (hash_index + 1) % head->size;
    }
    head->table[hash_index] = NULL;
    head->elem--;

    new_index = hash_index;
    while ((cobj = head->table[++hash_index % head->size]) != NULL)
    {
        ideal_index = head->obj_hash_func(cobj) % head->size;

        hash_index %= head->size;
        if (bs_hash_can_move_obj(ideal_index, new_index, head->size))
        {
            head->table[new_index] = cobj;
            head->table[hash_index] = NULL;
            new_index = hash_index;
        }
    }    
    return 0;
}

void *bs_hash_get(bs_hash_t *head,const void *key)
{
    unsigned int  hash_index;
    void         *obj;

    if (key == NULL)
        return NULL;

    hash_index = head->key_hash_func(key) % head->size;
    while ((obj = head->table[hash_index]) != NULL)
    {
        if (head->obj_key_cmp(obj, key))
            return obj;
        hash_index = (hash_index + 1) % head->size;
    }
    return NULL;    
}

void bs_hash_for_each(bs_hash_t *head, bs_hash_for_each_cb_func cb_func, void *data)
{
    int ex;

    for (ex=0; ex<head->size; ex++)
    {
        if (head->table[ex] == NULL)
            continue;
        cb_func(head->table[ex],data);                
    }
}

unsigned int bs_hash_update(const void *key_param, int key_len)
{
   unsigned int   a;
   unsigned int   len;
   unsigned char *key = (unsigned char *) key_param;

   /* Set up the internal state */
   len = (unsigned) key_len;
   a = 0 + len;

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

