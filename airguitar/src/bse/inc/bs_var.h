#ifndef BS_VAR_H
#define BS_VAR_H

#include "bs_type.h"
#include "bs_hash.h"
#include "bs_os.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BS_VAR_FLAG_FIXED 0x01 /* value size can not be changed */

typedef struct bs_var_entry_s
{
    unsigned char   type;
    unsigned char   flag;
    unsigned char   name_length;  /* 0..255 */
    unsigned char   value_length; /* 0..255 */
    char           *name;
    union
    {
        float f; /* 4 bytes */
        int   i; /* 4 bytes */
        char *s; /* 4 bytes */
    } value;
} bs_var_entry_t;

void bs_var_init();

char *bs_var_get_str(const char *name, char *value, int *value_size);

float bs_var_get_float(const char *name,float default_value);

int bs_var_get_int(const char *name,int default_value);

bs_err bs_var_set_str(const char *name,char *value);

bs_err bs_var_set_float(const char *name,float value);

bs_err bs_var_set_int(const char *name,int value);

bs_err bs_var_set_entry(bs_var_entry_t *entry);

void bs_var_parse(const char *line);

void bs_var_show(char *match);

#ifdef __cplusplus
}
#endif

#endif /* BS_VAR_H */
