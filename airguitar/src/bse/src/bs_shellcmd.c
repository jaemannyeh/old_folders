
#include "bs_list.h"
#include "bs_shell.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bs_list_t bs_shell_cmd_head;

bs_shell_cmd_t *bs_shell_find_cmd(const char *name)
{
    bs_shell_cmd_t *cmd;

    for (cmd = (bs_shell_cmd_t *)bs_list_first(&bs_shell_cmd_head); cmd; cmd = (bs_shell_cmd_t *)bs_list_next(&cmd->list))
        if (strcmp(cmd->name, name) == 0)
            break;

    return cmd;	
}

int bs_shell_add_cmd(const char *name, bs_shell_cmd_f func)
{
    bs_shell_cmd_t *cmd;

    cmd = bs_shell_find_cmd(name);
    if (cmd != NULL)
    {
        return -1;
    }

    cmd = malloc(sizeof(bs_shell_cmd_t));
	
	strncpy(cmd->name, name, sizeof(cmd->name)-1);
    cmd->func = func;

    bs_list_insert_last(&bs_shell_cmd_head,&cmd->list);

    return 0;
}

int bs_shellcmd_init()
{
    bs_list_init(&bs_shell_cmd_head);

    return 0;
}

