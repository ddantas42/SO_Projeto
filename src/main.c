#include <projeto.h>

void sig_handler(int signum)
{
	if (board == NULL)
		return;

	if (signum == SIGINT)
	{
		board->stop_signal = 1; // Sinaliza para os processos que eles devem parar
		if (DEBUG)
			printf("[DEBUG] SIGINT recebido, sinalizando para os processos pararem.\n");
	}
}

static pid_t create_process(t_sharedboard *board, void *(*thread_fun)(void *), int number_of_threads)
{
	pid_t pid = -1;
	pid = fork();
	if (pid < 0) // if fork fails
	{
		perror("fork");
		exit(EXIT_FAILURE);
	}
	if (pid == 0) // if we are in the child process
	{
		if (DEBUG)
			logger(MAIN_LOG, 0,"Processo de análise criado com PID %d\n", getpid());
		
		// ignora signal
		signal(SIGINT, SIG_IGN); // Ignora o sinal de interrupção (Ctrl+C) para evitar que o processo seja interrompido abruptamente
		thread_creator(board, thread_fun, number_of_threads);
		
		exit(EXIT_SUCCESS); // Certifique-se de sair do processo filho após a execução
	}
	// Father process continues here
	return pid;
}



t_sharedboard *board = NULL; // Declared as extern in projeto.h
int main()
{
	pid_t analysis_pid = -1, exploration_pid = -1;
	signal(SIGINT, sig_handler); // Ignora o sinal de interrupção (Ctrl+C) para evitar que o processo seja interrompido abruptamente
	initialize_sharedboard(&board); // Inicializa o tabuleiro compartilhado	
	initialize_results_csv(); // Cria o CSV de reconstrução antes dos forks
	fflush(NULL); // Evita que buffers de stdout sejam duplicados após fork

	analysis_pid = create_process(board, analysis_thread, NUMBER_OF_ANALYSIS_THREADS); // Cria o processo de análise
	exploration_pid = create_process(board, exploration_thread, NUMBER_OF_DRONES_THREADS); // Cria o processo da frota de drones

	waitpid(analysis_pid, NULL, 0); // Espera o processo de análise terminar
	waitpid(exploration_pid, NULL, 0); // Espera o processo de exploração terminar
	return 0;
}