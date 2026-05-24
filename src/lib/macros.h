#ifndef MACROS_H
#define MACROS_H

# define STORAGE_CAPACITY 5 // Capacidade máxima de armazenamento máximo do tabuleiro onde os drones depositam

// Drone Macros
# define NUMBER_OF_DRONES_THREADS 3 // Números total de drones a serem criados
# define DRONE_FINDING_TIME 5 // Tempo que o drone leva para encontrar um item (em segundos)

// Análise Macros
# define ANALYSIS_MAX_TIME 8 // Tempo máximo que a análise pode levar (em segundos, sendo o mínimo 1)
# define NUMBER_OF_ANALYSIS_THREADS 2 // Número de threads de análise a serem criadas

// definir se usamos CLOCK_MONOTONIC ou CLOCK_REALTIME para medir o tempo
// CLOCK_MONOTONIC é recomendado para medir intervalos de tempo, pois não é afetado por mudanças no relógio do sistema
// CLOCK_REALTIME pode ser usado para obter a hora atual, mas pode ser afetado por mudanças no relógio do sistema
# define CLOCK_ID CLOCK_MONOTONIC

# define DEBUG 1 // Define como 1 para habilitar mensagens de debug, ou 0 para desabilitar


#endif