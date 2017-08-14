/* pp_config.c - */

/************************************************************ include files */

#include "pp_base.h"

/****************************************************************** defines */

/******************************************************************** types */

/************************************************ extern & local prototypes */

/****************************************************************** globals */

/******************************************************************* locals */

#ifdef PP_CALC
    extern void pp_calc_init();
    static void *unused_pp_calc_init = pp_calc_init;
#endif

#ifdef PP_CLASS
    extern void pp_class_init();
    static void *unused_pp_class_init = pp_class_init;
#endif

#ifdef PP_HTTPD
    extern void pp_httpd_init();
    static void *unused_pp_httpd_init = pp_httpd_init;
#endif

#ifdef PP_NAMED_VAR
    extern void pp_named_var_init();
    static void *unused_pp_named_var_init = pp_named_var_init;
#endif

#ifdef PP_SHARED_TAG
    extern void pp_shared_tag_init();
    static void *unused_pp_shared_tag_init = pp_shared_tag_init;
#endif

#ifdef PP_SHELL
    extern void pp_shell_init();
    static void *unused_pp_shell_init = pp_shell_init;
#endif

#ifdef PP_TELNETD
    extern void pp_telnetd_init();
    static void *unused_pp_telnetd_init = pp_telnetd_init;
#endif

/*==================================================================== pp_init
 * description:
 * parameters:
 * returns:
 */

void pp_init()
{
    pp_shell_startup();
}
