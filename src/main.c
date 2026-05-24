#include <projeto.h>

void sigint_handler(int signum)
{
	if (signum == SIGINT && board != NULL)
	{
		board->stop_signal = 1; // Sinaliza para os processos que eles devem parar
		if (DEBUG)
			printf("[DEBUG] SIGINT recebido, sinalizando para os processos pararem.\n");
	}
}

static void create_analysis_process(t_sharedboard *board, pid_t analysis_pid)
{
	if (analysis_pid < 0) // if fork fails
	{
		perror("fork");
		exit(EXIT_FAILURE);
	}
	if (analysis_pid == 0) // if we are in the child process
	{
		if (DEBUG)
			printf("Processo de análise criado com PID %d\n", getpid());

		signal(SIGINT, sigint_handler); // Configura o handler para SIGINT no processo de análise
		analysis_process(board);
		
		exit(EXIT_SUCCESS); // Certifique-se de sair do processo filho após a execução
	}
	// Father process continues here
}



t_sharedboard *board = NULL; // Declared as extern in projeto.h
int main()
{
	pid_t analysis_pid = -1;
	signal(SIGINT, SIG_IGN); // Ignora o sinal de interrupção (Ctrl+C) para evitar que o processo seja interrompido abruptamente

	initialize_sharedboard(&board); // Inicializa o tabuleiro compartilhado	

	// fill with random for testing
	board->count = STORAGE_CAPACITY;
	for (int i = 0; i < STORAGE_CAPACITY; i++)
		init_random_sample(&board->samples[i]);
	
	analysis_pid = fork();
	create_analysis_process(board, analysis_pid); // Cria o processo de análise

	waitpid(analysis_pid, NULL, 0); // Espera o processo de análise terminar (opcional, dependendo do comportamento desejado)
	return 0;
}