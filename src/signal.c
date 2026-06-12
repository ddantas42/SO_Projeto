#include <projeto.h>

void sig_handler(int signum)
{
	if (signum == SIGINT)
	{
		if (board != NULL)
			board->stop_signal = 1; // Sinaliza para os processos que eles devem parar
		if (DEBUG)
			printf("[DEBUG] SIGINT recebido, stopping processes....\n");

		for (int i = 0; i < NUMBER_OF_ANALYSIS_THREADS; i++)
			sem_post(&board->available_samples);

		for (int i = 0; i < NUMBER_OF_DRONES_THREADS; i++)
		    sem_post(&board->available_slots);
	}

	if (signum == SIGTSTP)
	{
		if (board != NULL)
			board->pause_analysis = !board->pause_analysis; // Alterna o estado de pausa da análise
		if (DEBUG)
			printf("[DEBUG] SIGTSTP recebido, %s analysis....\n", board->pause_analysis ? "pausing" : "resuming");
	}
}

void register_sig(int signum, void (*handler)(int))
{
	struct sigaction sa;
	sa.sa_handler = handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(signum, &sa, NULL);
}