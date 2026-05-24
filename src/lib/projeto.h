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
	int	collectors_id;
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

	pthread_mutex_t mutex;

	volatile sig_atomic_t stop_signal; // Sinal para indicar que os processos devem parar
} t_sharedboard;

extern t_sharedboard *board; 

typedef struct s_thread_args
{
	t_sharedboard *board;
	int thread_id;
} t_thread_args;

// init_shm.c
void initialize_sharedboard(t_sharedboard **);

// analysis_process.c
void analysis_process(t_sharedboard *);

// fleet_process.c
void fleet_process(t_sharedboard *);

// log.c
void diff_timespec(struct timespec *, struct timespec *end, struct timespec *);
void logger(enum e_log_type , int , const char *, ...);

// sample.c
void init_random_sample(t_sample *);
void init_sample(t_sample *);

#endif