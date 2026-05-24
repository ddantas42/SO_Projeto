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

static void analyse_sample(t_sharedboard *board)
{
	t_sample *sample = &board->samples[board->out];
	
	clock_gettime(CLOCK_ID, &sample->begin_analising_time);
	logger(ANALISIS_LOG, "Analysing type %d\n", sample->item_type);
	
	sleep((rand() % 2) + 1); // Simula o tempo de análise entre 1 e 3 segundos
	
	logger(ANALISIS_LOG, "Finished analysing type %d\n", sample->item_type);
	clock_gettime(CLOCK_ID, &sample->end_analising_time);

	init_sample(sample); // Limpa a amostra após a análise para indicar que a posição está livre para novas amostras
	
}

void analisis_process(t_sharedboard *board)
{
	while (!board->stop_signal) // Loop principal do processo de análise, continua enquanto o sinal de parada não for acionado
	{
		// Verificar se há amostras para analisar
		if (board->count > 0)
		{
			analyse_sample(board);

			// Atualizar os índices e contagem do tabuleiro
			board->out = (board->out + 1) % STORAGE_CAPACITY; // Move para a próxima posição circularmente
			board->count--; // Decrementa a contagem de itens no tabuleiro
			
			print_current_board(board);

		}
		else if (board->count == 0)
		{
			logger(ANALISIS_LOG, "Waiting for samples...\n");
			sleep(1); // Evita que o processo de análise consuma 100% da CPU quando não há amostras para analisar
		}
	}
	if (DEBUG)
		printf("[DEBUG] Processo de análise com PID %d recebendo sinal de parada, encerrando...\n", getpid());
}