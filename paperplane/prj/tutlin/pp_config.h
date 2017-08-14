
#ifndef _PP_CONFIG_H_
#define _PP_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *
 */

#undef  PP_CYGWIN
#define PP_LINUX

#define PP_CALC                          /* PP_LD_FLG += -ly -lfl */

#undef  PP_CLASS

#define PP_HTTPD
#define PP_HTTPD_PORT 0x4854             /* "HT" = 0x4854 = 18516 */

#define PP_NAMED_VAR

#define PP_SHARED_TAG
#undef  PP_SHARED_TAG_USE_BERKELEY_DB    /* PP_LD_FLG += -ldb */

#define PP_SHELL                         /* PP_SHELL should always be defined */

#define PP_TELNETD
#define PP_TELNETD_PORT 0x5445           /* "TE" = 0x5445 = 21573 */

/*
 *
 */

#ifdef __cplusplus
}
#endif

#endif /* _PP_CONFIG_H_ */

