
#ifndef _PP_CONFIG_H_
#define _PP_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *
 */

#define PP_CALC

#define PP_CLASS

#define PP_HTTPD
#define PP_HTTPD_PORT 0x4854       /* "HT" = 0x4854 = 18516 */

#define PP_SHARED_TAG
#define PP_SHARED_TAG_USE_BERKELEY_DB

#define PP_SHELL                   /* PP_SHELL should be defined */

#define PP_TELNETD
#define PP_TELNETD_PORT 0x5445     /* "TE" = 0x5445 = 21573 */

/*
 *
 */

#ifdef __cplusplus
}
#endif

#endif /* _PP_CONFIG_H_ */

