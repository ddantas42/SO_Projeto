#include <projeto.h>

static t_sample generate_sample(int collector_id)
{
	t_sample new_sample;

	new_sample.collector_id = collector_id;
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

void *exploration_thread(void *arg)
{
	t_thread_args 			*thread_args = (t_thread_args *)arg;
	t_sharedboard 			*board = thread_args->board;
	t_sample				new_sample;
	int 					thread_id = thread_args->thread_id;

	logger(DRONE_LOG, thread_id, "Starting exploration thread %d...\n", thread_id);
	
	while (!board->stop_signal) // Loop principal do processo da frota, continua enquanto o sinal de parada não for acionado
	{
		sleep(DRONE_FINDING_TIME); // Simula o tempo entre as coletas de amostras pela frota de drones

		new_sample = generate_sample(thread_id); // Gera uma nova amostra com dados aleatórios
		
		sem_wait(&board->available_slots); // Aguarda até que haja um slot disponível para depósito de amostra

		pthread_mutex_lock(&board->board_mutex);
		
		deposit_sample(board, &new_sample); // Deposita a nova amostra no tabuleiro compartilhado
		
		pthread_mutex_unlock(&board->board_mutex);

		sem_post(&board->available_samples); // Sinaliza que há uma nova amostra disponível para análise

		logger(DRONE_LOG, thread_id, "Deposited sample of type %d\n", new_sample.item_type);
		
	}

	logger(DRONE_LOG, thread_id, "Stopping..\n");

	return NULL;
}