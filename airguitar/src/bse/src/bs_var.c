
#include "bs_var.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BS_VAR_DEBUG

#ifdef  BS_VAR_DEBUG

int bs_var_dbg_lvl = 0;

#define BS_VAR_DBG_OFF 0x00000000
#define BS_VAR_DBG_ALL 0xffffffff

#define BS_VAR_DBG(LVL, FMT, X1, X2, X3, X4, X5, X6) do { if (bs_var_dbg_lvl & BS_VAR_DBG_##LVL) printf (FMT, X1, X2, X3, X4, X5, X6); } while (0)

#else /*BS_VAR_DEBUG*/

#define BS_VAR_DBG(LVL, FMT, X1, X2, X3, X4, X5, X6) do {} while (0)

#endif /*BS_VAR_DEBUG*/


static bs_hash_t *bs_vars = NULL;

static unsigned int bs_var_hash_key(const char *key)
{
    return bs_hash_update(key, strlen(key));
}

static unsigned int bs_var_hash_obj(bs_var_entry_t *entry)
{
    return bs_var_hash_key(entry->name);
}

static int bs_var_hash_cmp(bs_var_entry_t *s, char *key)
{
    return strcmp(s->name, key) == 0;
}

void bs_var_init()
{
    if (bs_vars)
        return;

    bs_vars = bs_hash_new((bs_hash_obj_f)bs_var_hash_obj,(bs_hash_key_f)bs_var_hash_key,(bs_hash_cmp_f)bs_var_hash_cmp);

    bs_var_set_int("bs_vars_sizeof_int",sizeof(int));
    bs_var_set_int("bs_vars_sizeof_float",sizeof(float));

    bs_var_set_float("bs_vars_pi",3.14159265358979323846);

    /* 1st update */
    bs_var_set_int("bs_vars_hash_elem",bs_hash_elem(bs_vars));
    bs_var_set_int("bs_vars_hash_size",bs_hash_size(bs_vars));
    bs_var_set_int("bs_vars_hash_miss",bs_hash_miss(bs_vars));
}

static char **bs_var_get(const char *name, char *value, int *value_size)
{
    bs_var_entry_t  *entry;
    int              value_length;
    char           **ret;

    bs_mutex_lock(NULL);

    entry = bs_hash_get(bs_vars, name);
    if (entry == NULL)
    {
        ret = NULL;
        goto leave;
    }

    if (value == NULL || value_size == NULL)
    {
        ret = &entry->value.s;
        goto leave;
    }	    

    value_length = entry->value_length;
    if (value_length > *value_size)
    {
        *value_size = value_length; /* with NULL */
        ret = NULL;
        goto leave;
    }
    else
    {
        memcpy(value,entry->value.s,value_length); /* with NULL terminator */
        *value_size = value_length - 1; /* without NULL */
        ret = &entry->value.s;
        goto leave;
    }

leave:
    bs_mutex_unlock(NULL);
    return ret;
}

float *bs_var_get_float_ptr(const char *name)
{
    return (float *)bs_var_get(name, NULL, 0);
}

float bs_var_get_float(const char *name,float default_value)
{
    float *value;

    value = bs_var_get_float_ptr(name);
    if (value)
        return *value;
    else
        return default_value;
}

int *bs_var_get_int_ptr(const char *name)
{
    return (int *)bs_var_get(name, NULL, 0);
}

int bs_var_get_int(const char *name,int default_value)
{
    int *value;

    value = bs_var_get_int_ptr(name);
    if (value)
        return *value;
    else
        return default_value;
}

char *bs_var_get_str(const char *name, char *value, int *value_size)
{
    return *bs_var_get(name, value, value_size);
}

#define BS_VAR_HEAP_SIZE (64*1024)

static void *bs_var_malloc(size_t size)
{
	static char *bs_var_heap_start = NULL;
	static char *bs_var_heap_free = NULL;
	static char *bs_var_heap_end = NULL;

    if (bs_var_heap_start == NULL)
    {
        bs_var_heap_start = malloc(BS_VAR_HEAP_SIZE);
        if (bs_var_heap_start == NULL)
        {
            return NULL;
        }
		bs_var_heap_free = bs_var_heap_start;
        bs_var_heap_end = bs_var_heap_start + BS_VAR_HEAP_SIZE;
    }

    if (bs_var_heap_free+size < bs_var_heap_end)
    {
        char *block = bs_var_heap_free;

        if (size%4 == 0)
			bs_var_heap_free += size;
        else
            bs_var_heap_free += (size + (4-size%4));

        BS_VAR_DBG(ALL,"0x%08x %d %d\n",block,size,4-size%4,4,5,6);
        return block;
    }

	BS_VAR_DBG(ALL,"%d\n",size,2,3,4,5,6);
    return malloc(size);
}

static void bs_var_free(const char *ptr)
{
    /* no support */
}

static bs_err bs_var_set(const char *name, void *value, unsigned char type)
{
    bs_var_entry_t  *entry;
    unsigned char    name_length;  /* should be unsigned char */
    unsigned char    value_length; /* should be unsigned char */
    bs_err           ret = -1;

    if (name == NULL)
        return ret;
    if (name[0] == '#')
        return ret;
    if (value == NULL)
        return ret;

    switch (type)
    {
    case 'f':
        value_length = 0;
        break;
    case 'i':
        value_length = 0;
        break;
    case 's':
        value_length = strlen((char *)value) + 1; /* including NULL terminator */
        break;
    default:
        return ret;
    }

    name_length = strlen(name) + 1; /* including NULL terminator */

    bs_mutex_lock(NULL);

    /********************************/

    entry = bs_hash_get(bs_vars,name);
    if (entry)
    {
        if(entry->type != type)
            goto bs_var_set_failed; 

        if (entry->value_length < value_length)
        {
            if (entry->flag & BS_VAR_FLAG_FIXED)
                goto bs_var_set_failed;

            bs_hash_remove(bs_vars,entry);
            free(entry);
            entry = NULL;
        }
    }

    /********************************/

    if (entry == NULL)
    {
        if (type == 's')
            entry = malloc(sizeof(*entry) + name_length + value_length);
        else
            entry = bs_var_malloc(sizeof(*entry) + name_length + value_length);

        if(entry == NULL)
            goto bs_var_set_failed; 

        entry->name_length = name_length;
        entry->name = (char *)entry + sizeof(*entry);
        memcpy((void *)entry->name, name, name_length); /* including NULL terminator */

        entry->type = type;
    }

    /********************************/

    switch (type)
    {
    case 'f':
        entry->value.f = *(float *)value; /* (char *)*(float *)value */
        break;
    case 'i':
        entry->value.i = *(int *)value;
        break;
    case 's':
        entry->value_length = value_length;
        entry->value.s = entry->name + name_length;
        memcpy((void *)entry->value.s, value, value_length); /* including NULL terminator */
        break;
    }

    ret = bs_hash_add(bs_vars,entry);

bs_var_set_failed:
    bs_mutex_unlock(NULL);
    return ret;
}

bs_err bs_var_set_float(const char *name,float value)
{
    return bs_var_set(name,&value,'f');
}

bs_err bs_var_set_int(const char *name,int value)
{
    return bs_var_set(name,&value,'i');
}

bs_err bs_var_set_str(const char *name,char *value)
{
    return bs_var_set(name,&value[0],'s');
}

void bs_var_parse(const char *line)
{
    char    buff[256];
    char   *lasts;
    char   *name;
    char   *value;
    char   *type;

    strncpy(buff,line,sizeof(buff));

    name  = strtok_r(buff,"\t",&lasts);
    value = strtok_r(NULL,"\t",&lasts);
    type = strtok_r(NULL,"\t",&lasts);

    if (type == NULL)
        return;

    if (type[0] == 'f')
        bs_var_set_float(name,(float)atof(value));
    else if (type[0] == 'i')
        bs_var_set_int(name,atoi(value));
    else if (type[0] == 's')
        bs_var_set_str(name,value);
    else
        ;
}

bs_err bs_var_set_entry(bs_var_entry_t *entry)
{
    bs_err           ret = -1;

    bs_mutex_lock(NULL);

    if (bs_hash_get(bs_vars,entry->name))
    {
        goto bs_var_set_entry_failed; /* duplicated */
    }

    switch (entry->type)
    {
    case 'f':
        entry->value_length = 0;
        break;
    case 'i':
        entry->value_length = 0;
        break;
    case 's':
        entry->value_length = strlen(entry->value.s) + 1; /* including NULL terminator */
        break;
    default:
        goto bs_var_set_entry_failed; 
    }

	entry->flag |= BS_VAR_FLAG_FIXED; /* value size can not be changed */

    entry->name_length = strlen(entry->name) + 1;

    ret = bs_hash_add(bs_vars,entry);

bs_var_set_entry_failed:

    bs_mutex_unlock(NULL);
    return ret;
}

bs_err bs_var_unset(const char *name)
{
    bs_mutex_lock(NULL);

	bs_var_free(name); /* no support */

    bs_mutex_unlock(NULL);

    return -1;
}

void bs_var_from_file(const char *file_name)
{
    FILE          *from_file;
    char           line[256];

    from_file = fopen(file_name,"r");
    if (from_file == NULL)
    {
        return;
    }

    while (fgets(line,sizeof(line)-1,from_file))
    {
        if (line[strlen(line)-1] == 0x0a)
            line[strlen(line)-1] = 0x00;

        bs_var_parse(line);
    }

    fclose(from_file);
}

static int bs_vars_alloc_f;
static int bs_vars_alloc_i;
static int bs_vars_alloc_s;

static void bs_var_show_cb(bs_var_entry_t *entry,char *match)
{
    if (match == NULL)
    {
		switch (entry->type)
		{
		case 'f':
			printf("%s\t%f\t%c\n",entry->name,entry->value.f,entry->type);
			break;
		case 'i':
			printf("%s\t%d\t%c\n",entry->name,entry->value.i,entry->type);
			break;
		case 's':
			printf("%s\t%s\t%c\n",entry->name,entry->value.s,entry->type);
			break;
		}
    }
    else if (match == (char *)-1)
    {
		switch (entry->type)
		{
		case 'f':
			bs_vars_alloc_f += sizeof(*entry) + entry->name_length + entry->value_length;
			break;
		case 'i':
			bs_vars_alloc_i += sizeof(*entry) + entry->name_length + entry->value_length;
			break;
		case 's':
			bs_vars_alloc_s += sizeof(*entry) + entry->name_length + entry->value_length;
			break;
		}
    }
    else
    {
		if (NULL == strstr(entry->name,match))
			return;

		switch (entry->type)
		{
		case 'f':
			printf("%s\t%f\t%c\n",entry->name,entry->value.f,entry->type);
			break;
		case 'i':
			printf("%s\t%d\t%c\n",entry->name,entry->value.i,entry->type);
			break;
		case 's':
			printf("%s\t%s\t%c\n",entry->name,entry->value.s,entry->type);
			break;
		}
    }


}

void bs_var_show(char *match)
{
    bs_var_init(); /* do nothing if bs_vars is not null. */

    if (match == (char *)-1)
    {
        bs_vars_alloc_f = 0;
        bs_vars_alloc_i = 0;
        bs_vars_alloc_i = 0;
    }

	bs_hash_for_each(bs_vars,(bs_hash_for_each_cb_func)bs_var_show_cb,match);

    if (match == (char *)-1)
    {
        bs_var_set_int("bs_vars_alloc_f",bs_vars_alloc_f);
        bs_var_set_int("bs_vars_alloc_i",bs_vars_alloc_i);
        bs_var_set_int("bs_vars_alloc_s",bs_vars_alloc_s);
    }

    /* 2nd update */
    bs_var_set_int("bs_vars_hash_elem",bs_hash_elem(bs_vars));
    bs_var_set_int("bs_vars_hash_size",bs_hash_size(bs_vars));
    bs_var_set_int("bs_vars_hash_miss",bs_hash_miss(bs_vars));
}

