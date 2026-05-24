# include <projeto.h>

void analisis_process(t_sharedboard *board)
{
	while (board->count > 0)
	{
		// Verificar se há amostras para analisar
		if (board->count > 0) {
			// Analisar a amostra na posição 'out'
			t_sample *sample = &board->samples[board->out];
			
			// Registrar o tempo de início da análise
			clock_gettime(CLOCK_ID, &sample->begin_analising_time);
			
			// Simular o tempo de análise (pode ser ajustado conforme necessário)
			sleep((rand() % 3) + 1); // Simula o tempo de análise entre 1 e 3 segundos
			
			// Registrar o tempo de fim da análise
			clock_gettime(CLOCK_ID, &sample->end_analising_time);
			
			// Atualizar os índices e contagem do tabuleiro
			board->out = (board->out + 1) % STORAGE_CAPACITY; // Move para a próxima posição circularmente
			board->count--; // Decrementa a contagem de itens no tabuleiro
			
			if (DEBUG)
				printf("[DEBUG] Analisada amostra do coletor %d do tipo %d. Tempo de análise: %ld segundos e %ld nanosegundos.\n",
					sample->collectors_id, sample->item_type,
					sample->end_analising_time.tv_sec - sample->begin_analising_time.tv_sec,
					sample->end_analising_time.tv_nsec - sample->begin_analising_time.tv_nsec);
		}
	}
}