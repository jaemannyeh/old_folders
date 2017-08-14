
#include <stdlib.h>
#include <string.h>

int bs_run_cmd(char *cmd_str)
{
    char  *cmd_line;

    cmd_line = strdup(cmd_str);
    if (cmd_line == NULL)
    {
        return -1;
    }

    free(cmd_line);

    return 0;
}

