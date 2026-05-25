#include <projeto.h>

void thread_creator(t_sharedboard *board, void *(*thread_func)(void *), int number_of_threads)
{
	pthread_t analysis_thread_id[number_of_threads];
	t_thread_args thread_args[number_of_threads];

	srand(time(NULL) / getpid()); // Inicializa a semente do gerador de números aleatórios com base no tempo e no PID para garantir variedade entre processos
	
	// Criar as threads de análise
	for (int i = 0; i < number_of_threads; i++)
	{
		thread_args[i].board = board; // Atribui o ponteiro para o tabuleiro compartilhado
		thread_args[i].thread_id = i; // Atribui o ID da thread para fins de identificação (opcional)

		if (pthread_create(&analysis_thread_id[i], NULL, thread_func, (void *)&thread_args[i]) != 0)
		{
			perror("pthread_create");
			exit(EXIT_FAILURE);
		}
	}

	// Esperar as threads de análise terminarem
	for (int i = 0; i < number_of_threads; i++)
	{
		if (pthread_join(analysis_thread_id[i], NULL) != 0)
		{
			perror("pthread_join");
			exit(EXIT_FAILURE);
		}
	}
}
