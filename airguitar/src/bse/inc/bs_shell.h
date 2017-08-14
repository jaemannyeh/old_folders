
#ifndef BS_SHELL_H
#define BS_SHELL_H

#include "bs_list.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*bs_shell_cmd_f)(int argc,char **argv);

typedef struct bs_shell_cmd_s
{
    bs_list_t         list; /* should be first member */
    char               name[32];
    bs_shell_cmd_f    func;
} bs_shell_cmd_t;

#ifdef __cplusplus
}
#endif

#endif /* BS_SHELL_H */
