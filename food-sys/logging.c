#include "logging.h"

void syslog_init()
{
#ifdef SERVER
	char progname[] = "FOODSYS-SERVER";
#else
	char progname[] = "CLIENT";
#endif
	openlog(progname,LOG_ODELAY,LOG_LOCAL1);
}

