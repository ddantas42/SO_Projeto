#include <projeto.h>

/*
	@brief Função para inicializar o tabuleiro compartilhado
	@param board Ponteiro para o tabuleiro compartilhado a ser inicializado
*/
void initialize_sharedboard(t_sharedboard **board)
{
	int shmid = -1;

	// Criar uma área de memória compartilhada para o tabuleiro
	// IPC_PRIVATE indica que a chave é gerada de forma privada
	// sizeof(t_sharedboard) é o tamanho da memória a ser alocada
	// IPC_CREAT indica que a memória deve ser criada se não existir, e 0666 são as permissões de leitura e escrita para todos os usuários
	shmid = shmget(IPC_PRIVATE, sizeof(t_sharedboard), IPC_CREAT | 0666);
	if (shmid < 0) {
		perror("shmget");
		exit(EXIT_FAILURE);
	}

	// Attaching the shared memory segment to the process's address space
	*board = (t_sharedboard *)shmat(shmid, NULL, 0);
	if (*board == (t_sharedboard *)-1) {
		perror("shmat");
		exit(EXIT_FAILURE);
	}

	// initialize the shared board
	memset(*board, 0, sizeof(t_sharedboard)); // Limpa toda a estrutura para garantir que os valores sejam inicializados corretamente
	(*board)->board_mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER; // Inicializa o mutex para proteger o acesso ao tabuleiro
	(*board)->log_mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER; // Inicializa o mutex para proteger o acesso ao logger
	for (int i = 0; i < STORAGE_CAPACITY; i++)
		init_sample(&(*board)->samples[i]);

	if (DEBUG)
		logger(MAIN_LOG, 0,"Initialize_sharedboard finished | shmid %d | board %p.\n", shmid, (void *)(*board));
}