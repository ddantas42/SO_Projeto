#include "lib/projeto.h"

static void fill_sample(t_sample *sample)
{
	sample->collectors_id = rand() % NUMBER_OF_DRONES; // Atribui um ID de coletor aleatório
	sample->item_type = rand() % ITEM_TYPE_COUNT; // Atribui um tipo de item aleatório
	clock_gettime(CLOCK_ID, &sample->collected_time); // Registra o tempo de coleta
}

int main()
{
	t_sample samples[STORAGE_CAPACITY];
	for (int i = 0; i < STORAGE_CAPACITY; i++)
		fill_sample(&samples[i]);
	


	

	
	
	printf("Sup Drones!\n");
	return 0;
}