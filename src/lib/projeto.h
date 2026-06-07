#ifndef PROJETO_H
#define PROJETO_H

// Posix.1 requires this to be defined before including any headers to ensure that the correct features are exposed by the system headers.
# define _POSIX_C_SOURCE 200809L

# include <stdio.h>
# include <stdlib.h>
# include <time.h>
# include <sys/ipc.h>
# include <sys/shm.h>
# include <string.h>
# include <unistd.h>
# include <sys/wait.h>
# include <stdarg.h>
# include <signal.h>
# include <pthread.h>
# include <errno.h>
# include <stdbool.h>


# include "macros.h"


enum e_item_type
{
	ROCK,
	DIRT,
	SAND,
	OBSIDIAN,
	WATER,
	ITEM_TYPE_COUNT
};

enum e_log_type
{
	DRONE_LOG,
	ANALYSIS_LOG,
	MAIN_LOG
};


/*
struct timespec
{
	time_t tv_sec; // segundos
	long tv_nsec; // nanosegundos
};
*/

typedef struct s_sample
{
	int	collector_id;
	int	analyser_id;

	enum e_item_type item_type;

	struct timespec	collected_time;
	struct timespec	deposited_to_table_time;
	struct timespec	begin_analising_time;
	struct timespec	end_analising_time;

} t_sample;

typedef struct s_sharedboard
{
	t_sample samples[STORAGE_CAPACITY];
	
	int in; // Índice para o próximo item a ser depositado no tabuleiro
	int out; // Índice para o próximo item a ser analisado no tabuleiro
	int count; // Contagem de itens atualmente no tabuleiro

	pthread_mutex_t board_mutex; // Mutex para proteger o acesso ao tabuleiro
	pthread_mutex_t log_mutex; // Mutex para proteger o acesso ao logger

	volatile sig_atomic_t stop_signal; // Sinal para indicar que os processos devem parar
	volatile sig_atomic_t pause_analysis; // Sinal para indicar que a análise deve ser pausada

} t_sharedboard;

extern t_sharedboard *board; 

typedef struct s_thread_args
{
	t_sharedboard *board;
	int thread_id;
} t_thread_args;

// shm.c
void initialize_sharedboard(t_sharedboard **);
void free_sharedboard(t_sharedboard *);

// analysis_process.c
void *analysis_thread(void *);

// exploration_process.c
void *exploration_thread(void *);

// log.c
void diff_timespec(struct timespec *, struct timespec *end, struct timespec *);
void logger(enum e_log_type , int , const char *, ...);

// csv.c
void initialize_results_csv(void);
void append_sample_csv(const t_sample *);

// thread_creator.c
void thread_creator(t_sharedboard *, void *(*)(void *), int);

// sample.c
void init_sample(t_sample *);

// signal.c
void sig_handler(int);
void register_sig(int signum, void (*handler)(int));

#endif