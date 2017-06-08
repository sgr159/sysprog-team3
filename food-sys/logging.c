#include "logging.h"
#include <stdio.h>
#include <time.h>

void syslog_init()
{
#ifdef SERVER
	char progname[] = "FOODSYS-SERVER";
#else
	char progname[] = "CLIENT";
#endif
	openlog(progname,LOG_ODELAY,LOG_LOCAL1);
}

char * get_log_time(char *buffer)
{
    time_t timer;
    struct tm* tm_info;

    time(&timer);
    tm_info = localtime(&timer);

    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    return buffer;
}

void log_to_file(char * log_msg)
{
	FILE *f;
	f = fopen(LOG_FILE, "a+"); // a+ (create + append) option will allow appending which is useful in a log file
	if (f == NULL) {return;}
	fprintf(f,"%s",log_msg);
	fclose(f);
	return;
}
