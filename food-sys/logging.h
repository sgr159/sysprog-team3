#ifndef LOGGING
#include <syslog.h>

void syslog_init();

#define MAX_SYSLOG_BUF_SIZE 2048
char syslog_buf[MAX_SYSLOG_BUF_SIZE];

#define debug_print(fmt, ...) 								\
        do { printf("%s:%d:%s(): " fmt "\n", __FILE__, 					\
	                         __LINE__, __func__, __VA_ARGS__);  } while (0);

#define LOG(LEVEL,MSG,...)									\
	debug_print(MSG,__VA_ARGS__)							\
	syslog(LEVEL,MSG,__VA_ARGS__);

#endif
