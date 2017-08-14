#ifndef BS_HASH_H
#define BS_HASH_H

#include "bs_type.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int (*bs_hash_obj_f)(const void *obj);

typedef unsigned int (*bs_hash_key_f)(const void *key);

typedef unsigned int (*bs_hash_cmp_f)(const void *obj, const void *key);

typedef void (*bs_hash_for_each_cb_func)(void *obj,void *data);

typedef struct bs_hash_s
{
    bs_hash_obj_f   obj_hash_func;
    bs_hash_key_f   key_hash_func;
    bs_hash_cmp_f   obj_key_cmp;
    unsigned int     miss;	
    unsigned int     elem;
    unsigned int     size;
    void           **table;
} bs_hash_t;

unsigned int bs_hash_miss(bs_hash_t *head);

unsigned int bs_hash_elem(bs_hash_t *head);

unsigned int bs_hash_size(bs_hash_t *head);

bs_hash_t *bs_hash_new(bs_hash_obj_f obj_hash_func,bs_hash_key_f key_hash_func,bs_hash_cmp_f obj_key_cmp);

bs_err bs_hash_add(bs_hash_t *head, void *obj);

int bs_hash_remove(bs_hash_t *head, void *obj);

void *bs_hash_get(bs_hash_t *head, const void *key);

void bs_hash_for_each(bs_hash_t *head, bs_hash_for_each_cb_func cb_func, void *data);

unsigned int bs_hash_update(const void *key_param, int key_len);

#ifdef __cplusplus
}
#endif

#endif /* BS_HASH_H */
