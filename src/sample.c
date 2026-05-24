#include <projeto.h>

// initializes sample with default values
void init_sample(t_sample *sample)
{
	sample->collectors_id = -1;
	sample->item_type = ITEM_TYPE_COUNT; // Valor inválido para indicar que o tipo do item não foi definido
	memset(&sample->collected_time, 0, sizeof(struct timespec));
	memset(&sample->deposited_to_table_time, 0, sizeof(struct timespec));
	memset(&sample->begin_analising_time, 0, sizeof(struct timespec));
	memset(&sample->end_analising_time, 0, sizeof(struct timespec));
}

/*
	@brief Função para inicializar uma amostra com dados aleatórios para fins de teste
	@param sample Ponteiro para a amostra a ser inicializada
*/
void init_random_sample(t_sample *sample)
{
	sample->collectors_id = rand() % NUMBER_OF_DRONES;
	sample->item_type = rand() % ITEM_TYPE_COUNT;
	clock_gettime(CLOCK_ID, &sample->collected_time);
	clock_gettime(CLOCK_ID, &sample->deposited_to_table_time);
	memset(&sample->begin_analising_time, 0, sizeof(struct timespec));
	memset(&sample->end_analising_time, 0, sizeof(struct timespec));
}