# include <projeto.h>

// Gets sample from the board and resets the space of the board. 
static t_sample get_sample(t_sharedboard *board)
{
	t_sample *sample = &board->samples[board->out];
	t_sample local_sample = *sample;
	init_sample(sample); // Reseta a amostra no tabuleiro para indicar que o espaço está vazio

	// Atualizar os índices e contagem do tabuleiro
	board->out = (board->out + 1) % STORAGE_CAPACITY; // Move para a próxima posição circularmente
	board->count--;

	return local_sample;
}

// simulates analysis of sample and puts timestamps for the sample.
static void analyse_sample(t_sample *sample, int thread_id)
{
	int steps = (rand() % (ANALYSIS_MAX_TIME * 100 - 99)) + 100; // 100 → MAX_TIME (0.01s steps scaled)
	struct timespec analysis_time = { .tv_sec = steps / 100, .tv_nsec = (steps % 100) * 10000000L }; // Tempo de análise entre 1 e 3 segundos
	
	clock_gettime(CLOCK_ID, &sample->begin_analising_time);
	logger(ANALYSIS_LOG, thread_id, "Analysing type %d\n", sample->item_type);
	
	nanosleep(&analysis_time, NULL); // Simula o tempo de análise entre 1 e 3 segundos
	
	logger(ANALYSIS_LOG, thread_id, "Finished analysing type %d\n", sample->item_type);
	clock_gettime(CLOCK_ID, &sample->end_analising_time);

	sample->analyser_id = thread_id; // Atribui o ID da thread de análise à amostra para fins de registro

}

void *analysis_thread(void *arg)
{
	t_thread_args 	*thread_args = (t_thread_args *)arg;
	t_sharedboard 	*board = thread_args->board;
	t_sample 		local_sample;
	int 			thread_id = thread_args->thread_id;

	while (!board->stop_signal) // Loop principal do processo de análise, continua enquanto o sinal de parada não for acionado
	{
		pthread_mutex_lock(&board->board_mutex);
		if (board->count <= 0) // thread safety check
		{
			pthread_mutex_unlock(&board->board_mutex); // Desbloqueia o mutex antes de continuar
			logger(ANALYSIS_LOG, thread_id, "Waiting for samples...\n");
			sleep(1); // Evita que o processo de análise consuma 100% da CPU quando não há amostras para analisar
			continue; // Não há amostras para analisar, continua para a próxima iteração do loop
		}
		local_sample = get_sample(board); // Obtém uma amostra do tabuleiro para análise
		pthread_mutex_unlock(&board->board_mutex);

		analyse_sample(&local_sample, thread_id);
		append_sample_csv(&local_sample);

		// print_sample(&local_sample, thread_id);
		init_sample(&local_sample); // Limpa a amostra local para a próxima análise
		
		while (board->pause_analysis && !board->stop_signal) // Loop de pausa, continua enquanto o sinal de pausa estiver ativo e o sinal de parada não for acionado
		{
			logger(ANALYSIS_LOG, thread_id, "Analysis paused. Waiting to resume...\n");
			sleep(1); // Evita que o processo de análise consuma 100% da CPU enquanto está pausado
		}
	}
	if (DEBUG)
		logger(ANALYSIS_LOG, thread_id, "Stopping..\n");

	return NULL;
}