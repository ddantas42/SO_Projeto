# include <projeto.h>

static void print_current_board(t_sharedboard *board)
{
	for (int i = 0; i < STORAGE_CAPACITY; i++)
	{
		if (i > 0)
			printf(" | ");

		if (board->samples[i].collectors_id != -1)
			printf("[O]");
		else
			printf("[X]");
	}
	printf("\n");
}

// Gets sample from the board and resets the space of the board. 
static t_sample get_sample(t_sharedboard *board)
{
	t_sample *sample = &board->samples[board->out];
	t_sample local_sample = *sample;
	init_sample(sample);

	// Atualizar os índices e contagem do tabuleiro
	board->out = (board->out + 1) % STORAGE_CAPACITY; // Move para a próxima posição circularmente
	board->count--;

	return local_sample;
}

// simulates analysis of sample and puts timestamps for the sample.
static void analyse_sample(t_sample *sample, int thread_id)
{
	int steps = (rand() % 201) + 100; // 100 → 300 (0.01s steps scaled)
	struct timespec analysis_time = { .tv_sec = steps / 100, .tv_nsec = (steps % 100) * 10000000L }; // Tempo de análise entre 1 e 3 segundos
	
	clock_gettime(CLOCK_ID, &sample->begin_analising_time);
	logger(ANALYSIS_LOG, thread_id, "Analysing type %d\n", sample->item_type);
	
	nanosleep(&analysis_time, NULL); // Simula o tempo de análise entre 1 e 3 segundos
	
	logger(ANALYSIS_LOG, thread_id, "Finished analysing type %d\n", sample->item_type);
	clock_gettime(CLOCK_ID, &sample->end_analising_time);

}

static void print_sample(t_sample *sample, int thread_id)
{
	struct timespec *begin = &sample->begin_analising_time;
	struct timespec *end = &sample->end_analising_time;
	double duration = 0.0;

	if (end->tv_sec > 0 || end->tv_nsec > 0) {
		duration = (double)(end->tv_sec - begin->tv_sec) +
				   (double)(end->tv_nsec - begin->tv_nsec) / 1e9;
		if (duration < 0)
			duration = 0.0;
	}

	logger(ANALYSIS_LOG, thread_id,
		   "Sample collector_id=%d, Type=%d, Duration=%.3f s\n",
		   sample->collectors_id, sample->item_type, duration);
}

void *analysis_thread(void *arg)
{
	t_analysis_thread_args 	*thread_args = (t_analysis_thread_args *)arg;
	t_sharedboard 			*board = thread_args->board;
	t_sample 				local_sample;
	int 					thread_id = thread_args->thread_id;

	while (!board->stop_signal) // Loop principal do processo de análise, continua enquanto o sinal de parada não for acionado
	{
		pthread_mutex_lock(&board->mutex);
		if (board->count <= 0) // thread safety check
		{
			pthread_mutex_unlock(&board->mutex); // Desbloqueia o mutex antes de continuar
			logger(ANALYSIS_LOG, thread_id, "Waiting for samples...\n");
			sleep(1); // Evita que o processo de análise consuma 100% da CPU quando não há amostras para analisar
			continue; // Não há amostras para analisar, continua para a próxima iteração do loop
		}
		local_sample = get_sample(board); // Obtém uma amostra do tabuleiro para análise
		pthread_mutex_unlock(&board->mutex);

		analyse_sample(&local_sample, thread_id);

		print_sample(&local_sample, thread_id);
		init_sample(&local_sample); // Limpa a amostra local para a próxima análise
		
		if (DEBUG)
			print_current_board(board);
	}
	if (DEBUG)
		logger(ANALYSIS_LOG, thread_id, "Stopping..\n");

	return NULL;
}


void analysis_process(t_sharedboard *board)
{
	pthread_t analysis_thread_id[NUMBER_OF_ANALYSIS_THREADS];
	t_analysis_thread_args thread_args[NUMBER_OF_ANALYSIS_THREADS];

	srand(time(NULL) / getpid()); // Inicializa a semente do gerador de números aleatórios com base no tempo e no PID para garantir variedade entre processos
	
	// Criar as threads de análise
	for (int i = 0; i < NUMBER_OF_ANALYSIS_THREADS; i++)
	{
		thread_args[i].board = board; // Atribui o ponteiro para o tabuleiro compartilhado
		thread_args[i].thread_id = i; // Atribui o ID da thread para fins de identificação (opcional)

		if (pthread_create(&analysis_thread_id[i], NULL, analysis_thread, (void *)&thread_args[i]) != 0)
		{
			perror("pthread_create");
			exit(EXIT_FAILURE);
		}
	}

	// Esperar as threads de análise terminarem
	for (int i = 0; i < NUMBER_OF_ANALYSIS_THREADS; i++)
	{
		if (pthread_join(analysis_thread_id[i], NULL) != 0)
		{
			perror("pthread_join");
			exit(EXIT_FAILURE);
		}
	}
}
