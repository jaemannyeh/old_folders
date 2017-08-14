#ifndef BS_LOG_H
#define BS_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 *===========================================================================
 *                         syslog application macro API
 *===========================================================================
 * How to use:
 * ===========
 * The following two macros must be defined by the application, e.g.:
 *
 *    #define BS_LOG_PRIORITY         BS_LOG_DEBUG
 *    #define BS_LOG_FACILITY         BS_LOG_FTP
 */
 
#define BS_LOG0(x,fm)           \
    do { if ((BS_LOG_PRIORITY >= BS_LOG_ ## x)) \
        bs_log(BS_LOG_FACILITY | BS_LOG_ ## x, fm); } while((0))
#define BS_LOG1(x,fm,a)           \
    do { if ((BS_LOG_PRIORITY >= BS_LOG_ ## x)) \
        bs_log(BS_LOG_FACILITY | BS_LOG_ ## x, fm,a); } while((0))
#define BS_LOG9(x,fm,a,b,c,d,e,f,g,i,j)           \
    do { if ((BS_LOG_PRIORITY >= BS_LOG_ ## x)) \
        bs_log(BS_LOG_FACILITY | BS_LOG_ ## x, fm,a,b,c,d,e,f,g,i,j); } while((0))

#define	BS_LOG_EMERG	     0
#define	BS_LOG_CRIT	         1
#define	BS_LOG_ERR	         2
#define	BS_LOG_WARNING       3
#define	BS_LOG_NOTICE        4
#define	BS_LOG_INFO          5
#define	BS_LOG_DBG	         6


#define BS_LOG_CIS             (5 << 3)

void bs_log_init();

void bs_log(unsigned int priority, const char *format, ...);

void bs_log_show(int num,char *match);


#ifdef __cplusplus
}
#endif

#endif /* BS_LOG_H */
