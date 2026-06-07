#include <projeto.h>

// initializes sample with default values
void init_sample(t_sample *sample)
{
	sample->collector_id = -1;
	sample->analyser_id = -1; // Valor inválido para indicar que o analisador não foi definido
	sample->item_type = ITEM_TYPE_COUNT; // Valor inválido para indicar que o tipo do item não foi definido
	memset(&sample->collected_time, 0, sizeof(struct timespec));
	memset(&sample->deposited_to_table_time, 0, sizeof(struct timespec));
	memset(&sample->begin_analising_time, 0, sizeof(struct timespec));
	memset(&sample->end_analising_time, 0, sizeof(struct timespec));
}
