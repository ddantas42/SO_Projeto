#include <projeto.h>

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
		register_sig(SIGINT, SIG_IGN); // Registra o manipulador de sinal para SIGINT (Ctrl+C) no processo filho
		register_sig(SIGTSTP, SIG_IGN); // Registra o manipulador de sinal para SIGTSTP (Ctrl+Z) no processo filho

		if (DEBUG)
			logger(MAIN_LOG, 0,"Processo de análise criado com PID %d\n", getpid());
		
		thread_creator(board, thread_fun, number_of_threads);
		
		exit(EXIT_SUCCESS); // Certifique-se de sair do processo filho após a execução
	}
	// Father process continues here
	return pid;
}

static void wait_for_child(pid_t pid)
{
	int status;

	while (waitpid(pid, &status, 0) == -1) // Espera o processo filho terminar
	{
		if (errno == EINTR)
			continue;
		perror("waitpid");
		exit(EXIT_FAILURE);
	}
}

t_sharedboard *board = NULL; // Declared as extern in projeto.h
int main()
{
	pid_t analysis_pid = -1;
	pid_t exploration_pid = -1;

	register_sig(SIGINT,sig_handler); // Registra o manipulador de sinal para SIGINT (Ctrl+C)
	register_sig(SIGTSTP,sig_handler); // Configura o manipulador de sinal para SIGSTP (Ctrl+Z)

	initialize_results_csv(); // Cria o CSV de reconstrução antes dos forks
	initialize_sharedboard(&board); // Inicializa o tabuleiro compartilhado	
	fflush(NULL); // Evita que buffers de stdout sejam duplicados após fork

	analysis_pid = create_process(board, analysis_thread, NUMBER_OF_ANALYSIS_THREADS); // Cria o processo de análise
	exploration_pid = create_process(board, exploration_thread, NUMBER_OF_DRONES_THREADS); // Cria o processo da frota de drones

	wait_for_child(analysis_pid); // Espera o processo de análise terminar
	wait_for_child(exploration_pid); // Espera o processo de exploração terminar

	// free board
	free_sharedboard(board);
	return 0;
}