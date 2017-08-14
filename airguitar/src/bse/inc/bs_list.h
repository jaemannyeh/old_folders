#ifndef BS_LIST_H
#define BS_LIST_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct bs_list_s
{
  struct bs_list_s   *next;
  struct bs_list_s   *prev;
  struct bs_list_s   *head;
  unsigned int         size;
} bs_list_t;

void bs_list_init(bs_list_t *head);

bs_list_t *bs_list_first(bs_list_t *head);

bs_list_t *bs_list_last(bs_list_t *head);

bs_list_t *bs_list_next(bs_list_t *entry);

bs_list_t *bs_list_prev(bs_list_t *entry);

void bs_list_insert_first(bs_list_t *head,bs_list_t *entry);

void bs_list_insert_last(bs_list_t *head,bs_list_t *entry);

void bs_list_insert_before(bs_list_t *inlist,bs_list_t *entry);

void bs_list_insert_after(bs_list_t *inlist,bs_list_t *entry);

void bs_list_remove(bs_list_t *entry);

bs_list_t *bs_list_unlink_head(bs_list_t *head);

#ifdef __cplusplus
}
#endif

#endif /* BS_LIST_H */
