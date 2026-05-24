#include <projeto.h>

/*
	@brief Função para inicializar uma amostra com dados aleatórios para fins de teste
	@param sample Ponteiro para a amostra a ser inicializada
*/
static void init_random_sample(t_sample *sample)
{
	sample->collectors_id = rand() % NUMBER_OF_DRONES;
	sample->item_type = rand() % ITEM_TYPE_COUNT;
	clock_gettime(CLOCK_ID, &sample->collected_time);
	clock_gettime(CLOCK_ID, &sample->deposited_to_table_time);
	memset(&sample->begin_analising_time, 0, sizeof(struct timespec));
	memset(&sample->end_analising_time, 0, sizeof(struct timespec));
}

static void create_analisis_process(t_sharedboard *board, pid_t analisis_pid)
{
	if (analisis_pid < 0) // if fork fails
	{
		perror("fork");
		exit(EXIT_FAILURE);
	}
	if (analisis_pid == 0) // if we are in the child process
	{
		if (DEBUG)
			printf("Processo de análise criado com PID %d\n", getpid());
		analisis_process(board);
		exit(EXIT_SUCCESS); // Certifique-se de sair do processo filho após a execução
	}
	// Father process continues here
}

int main()
{
	t_sharedboard *board = NULL; 
	pid_t analisis_pid = -1;

	initialize_sharedboard(&board); // Inicializa o tabuleiro compartilhado	

	// fill with random for testing
	board->count = STORAGE_CAPACITY;
	for (int i = 0; i < STORAGE_CAPACITY; i++)
		init_random_sample(&board->samples[i]);
	
	analisis_pid = fork();
	create_analisis_process(board, analisis_pid); // Cria o processo de análise

	wait(NULL); // Espera o processo de análise terminar (opcional, dependendo do comportamento desejado)
	return 0;
}