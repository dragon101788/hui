#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "autoconf.h"

#ifdef CONFIG_DEBUG
#define debug(a,...)	fprintf(stderr,a,##__VA_ARGS__)
#else
#define debug(a,...)
#endif

#if defined(CONFIG_LOG_I)
#define log_i(args,...) (printf("I: "), printf(args,##__VA_ARGS__))//info
#define log_w(args,...) (printf("W: "), printf(args,##__VA_ARGS__))//warming
#define log_s(args,...) (printf("S: "), printf(args,##__VA_ARGS__))//state
#define log_e(args,...) (printf("E: "), printf(args,##__VA_ARGS__))//error
#elif defined(CONFIG_LOG_W)
#define log_i(args,...)
#define log_w(args,...) (printf("W: "), printf(args,##__VA_ARGS__))
#define log_s(args,...) (printf("S: "), printf(args,##__VA_ARGS__))
#define log_e(args,...) (printf("E: "), printf(args,##__VA_ARGS__))
#elif defined(CONFIG_LOG_S)
#define log_i(args,...)
#define log_w(args,...)
#define log_s(args,...) (printf("S: "), printf(args,##__VA_ARGS__))
#define log_e(args,...) (printf("E: "), printf(args,##__VA_ARGS__))
#elif defined(CONFIG_LOG_E)
#define log_i(args,...)
#define log_w(args,...)
#define log_s(args,...)
#define log_e(args,...) (printf("E: "), printf(args,##__VA_ARGS__))

#else
#define log_i(args,...)
#define log_w(args,...)
#define log_e(args,...)
#define log_s(args,...)
#endif


#endif //__DEBUG_H__
