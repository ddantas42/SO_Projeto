#include <projeto.h>

static t_sample generate_sample()
{
	t_sample new_sample;

	new_sample.collectors_id = rand() % NUMBER_OF_DRONES_THREADS;
	new_sample.item_type = rand() % ITEM_TYPE_COUNT;
	clock_gettime(CLOCK_ID, &new_sample.collected_time);
	
	memset(&new_sample.deposited_to_table_time, 0, sizeof(struct timespec));
	memset(&new_sample.begin_analising_time, 0, sizeof(struct timespec));
	memset(&new_sample.end_analising_time, 0, sizeof(struct timespec));

	return new_sample;
}

void deposit_sample(t_sharedboard *board, t_sample *sample)
{
	clock_gettime(CLOCK_ID, &sample->deposited_to_table_time);
	board->samples[board->in] = *sample; // Deposita a amostra no tabuleiro na posição atual de entrada

	// Atualizar os índices e contagem do tabuleiro
	board->in = (board->in + 1) % STORAGE_CAPACITY; // Move para a próxima posição circularmente
	board->count++;
}

// static void print_current_board(t_sharedboard *board)
// {
// 	for (int i = 0; i < STORAGE_CAPACITY; i++)
// 	{
// 		if (i > 0)
// 			printf(" | ");

// 		if (board->samples[i].collectors_id != -1)
// 			printf("[O]");
// 		else
// 			printf("[X]");
// 	}
// 	printf("\n");
// }


void *exploration_thread(void *arg)
{
	t_thread_args 			*thread_args = (t_thread_args *)arg;
	t_sharedboard 			*board = thread_args->board;
	t_sample				new_sample;
	int 					thread_id = thread_args->thread_id;

	while (!board->stop_signal) // Loop principal do processo da frota, continua enquanto o sinal de parada não for acionado
	{
		sleep(DRONE_FINDING_TIME); // Simula o tempo entre as coletas de amostras pela frota de drones

		new_sample = generate_sample(); // Gera uma nova amostra com dados aleatórios
		
		pthread_mutex_lock(&board->board_mutex);
		if (board->count >= STORAGE_CAPACITY) // thread safety check
		{
			pthread_mutex_unlock(&board->board_mutex); // Desbloqueia o mutex antes de continuar
			logger(DRONE_LOG, thread_id, "Waiting for space on the board...\n");
			continue ; // O tabuleiro está cheio, continua para a próxima iteração do loop
		}
		
		deposit_sample(board, &new_sample); // Deposita a nova amostra no tabuleiro compartilhado
		// print_current_board(board); // Imprime o estado atual do tabuleiro para fins de debug
		pthread_mutex_unlock(&board->board_mutex);

		logger(DRONE_LOG, thread_id, "Deposited sample of type %d\n", new_sample.item_type);
		
	}

	logger(DRONE_LOG, thread_id, "Stopping..\n");

	return NULL;
}