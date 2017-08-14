
#include "bs_list.h"

#include <stdio.h>

void bs_list_init(bs_list_t *head)
{
    head->next = head;
    head->prev = head;
    head->head = head;
    head->size = 0;
}

bs_list_t *bs_list_first(bs_list_t *head)
{
    if(head->next != head)
        return head->next;
    else
        return NULL;
}

bs_list_t *bs_list_last(bs_list_t *head)
{
    if(head->prev != head)
        return head->prev;
    else
        return NULL;
}

bs_list_t *bs_list_next(bs_list_t *entry)
{
    if(entry->next != entry->head)
        return entry->next;
    else
        return NULL;
}

bs_list_t *bs_list_prev(bs_list_t *entry)
{
    if(entry->prev != entry->head)
        return entry->prev;
    else
        return NULL;
}

void bs_list_insert_first(bs_list_t *head,bs_list_t *entry)
{
    entry->next = head->next;
    entry->prev = head;
    entry->next->prev = entry;
    entry->prev->next = entry;

    entry->head = head;
    head->size++;
}

void bs_list_insert_last(bs_list_t *head,bs_list_t *entry)
{
    entry->prev = head->prev;
    entry->next = head;
    entry->next->prev = entry;
    entry->prev->next = entry;

    entry->head = head;
    head->size++;
}

void bs_list_insert_before(bs_list_t *inlist,bs_list_t *entry)
{
}

void bs_list_insert_after(bs_list_t *inlist,bs_list_t *entry)
{
}

void bs_list_remove(bs_list_t *entry)
{
    entry->next->prev = entry->prev;
    entry->prev->next = entry->next;

    entry->head->size--;
    entry->head = NULL;
}

bs_list_t *bs_list_unlink_head(bs_list_t *head)
{
    return NULL;
}

