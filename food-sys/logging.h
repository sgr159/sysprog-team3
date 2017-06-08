#ifndef LOGGING
#include <syslog.h>

void syslog_init();

char * get_log_time(char * buffer);
void log_to_file(char * buffer);

#define LOG_FILE "foodsys.log"
#define MAX_SYSLOG_BUF_SIZE 2048

#define debug_print(LEVEL,fmt, ...) 							\
        do { 	char time_buffer[26];							\
         	char log_buffer[MAX_SYSLOG_BUF_SIZE];					\
		snprintf(log_buffer,sizeof(log_buffer),"%s : %s:%d:%s(): " fmt "\n",    \
				get_log_time(time_buffer),__FILE__,			\
	                         __LINE__, __func__, __VA_ARGS__);  			\
		log_to_file(log_buffer);						\
		if(LEVEL<=LOG_INFO)							\
			printf("%s",log_buffer);					\
	} while (0);

#define LOG(LEVEL,MSG,...)								\
	debug_print(LEVEL,MSG,__VA_ARGS__)						\
	syslog(LEVEL,MSG,__VA_ARGS__);

#endif
