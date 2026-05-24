#include <projeto.h>

void logger(enum e_log_type type,  const char *format, ...)
{
	va_list args;
	va_start(args, format);
	
	if (type == DRONE_LOG)
		printf("\t\t[DRONE] ");
	else if (type == ANALISIS_LOG)
		printf("\t\t\t\t[ANALISIS] ");
	else if (type == MAIN_LOG)
		printf("[MAIN] ");

	vprintf(format, args);
	

	va_end(args);
}