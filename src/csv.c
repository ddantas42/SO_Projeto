#include <projeto.h>

static void write_csv_header(FILE *file)
{
	fprintf(file, "analyser_id,collector_id,item_type,collected_sec,collected_nsec,deposited_sec,deposited_nsec,analysis_begin_sec,analysis_begin_nsec,analysis_end_sec,analysis_end_nsec\n");
}

void initialize_results_csv(void)
{
	FILE *file = fopen(CSV_FILE_PATH, "w");

	if (file == NULL)
	{
		perror("fopen");
		exit(EXIT_FAILURE);
	}
	write_csv_header(file);
	fclose(file);
}

void append_sample_csv(const t_sample *sample)
{
	FILE *file;

	if (board == NULL || sample == NULL)
		return;

	pthread_mutex_lock(&board->log_mutex);
	file = fopen(CSV_FILE_PATH, "a");
	if (file == NULL)
	{
		pthread_mutex_unlock(&board->log_mutex);
		perror("fopen");
		return;
	}
	fprintf(file,
		"%d,%d,%d,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld\n",
		sample->analyser_id,
		sample->collector_id,
		sample->item_type,
		(long)sample->collected_time.tv_sec,
		sample->collected_time.tv_nsec,
		(long)sample->deposited_to_table_time.tv_sec,
		sample->deposited_to_table_time.tv_nsec,
		(long)sample->begin_analising_time.tv_sec,
		sample->begin_analising_time.tv_nsec,
		(long)sample->end_analising_time.tv_sec,
		sample->end_analising_time.tv_nsec);
	fflush(file);
	fclose(file);
	pthread_mutex_unlock(&board->log_mutex);
}