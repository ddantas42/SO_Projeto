#include <projeto.h>

void diff_timespec(struct timespec *result, struct timespec *end, struct timespec *start)
{
	result->tv_sec = end->tv_sec - start->tv_sec;
	result->tv_nsec = end->tv_nsec - start->tv_nsec;

	if (result->tv_nsec < 0)
	{
		result->tv_sec -= 1;
		result->tv_nsec += 1000000000L;
	}
}

void logger(enum e_log_type type, int id, const char *format, ...)
{
	va_list args;
	va_start(args, format);

	static struct timespec start_time = {0, 0};
	struct timespec now;
	struct timespec diff;

	if (start_time.tv_sec == 0 && start_time.tv_nsec == 0)
		clock_gettime(CLOCK_ID, &start_time);

	clock_gettime(CLOCK_ID, &now);
	diff_timespec(&diff, &now, &start_time);

	long minutes = diff.tv_sec / 60;
	long seconds = diff.tv_sec % 60;
	long millis = diff.tv_nsec / 1000000;

	pthread_mutex_lock(&board->log_mutex); // Protege o acesso ao logger para evitar mensagens de log misturadas
	if (type == DRONE_LOG)
		printf("[%02ld.%02ld.%04ld]\t\t[DRONE %d] ", minutes, seconds, millis, id);
	else if (type == ANALYSIS_LOG)
		printf("[%02ld.%02ld.%04ld]\t\t\t\t[ANALYSIS %d] ", minutes, seconds, millis, id);
	else if (type == MAIN_LOG)
		printf("[%02ld.%02ld.%04ld][MAIN] ", minutes, seconds, millis);

	vprintf(format, args);
	pthread_mutex_unlock(&board->log_mutex); // Desbloqueia o mutex do logger após imprimir a mensagem
	va_end(args);
}